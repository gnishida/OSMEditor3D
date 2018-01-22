#version 420
#extension GL_EXT_gpu_shader4 : enable

layout(location = 0)out vec4 outputF;

uniform vec3 windSizeAspect;
//camera
uniform vec3 camPos;
uniform vec3 camDir;
uniform vec3 camUU;
uniform vec3 camVV;
// sun
uniform vec2 sunValues;//alt azimuth

const float PI = 3.14159265358979323846;
const float PI_2 = 1.57079632679489661923;
const float PI_4 = 0.785398163397448309616;

vec3 vGammaPowerInverse = vec3(2.2);
vec3 vGammaPower = vec3(0.454545);

vec3 linear_to_gamma(vec3 c)
{
	return pow(clamp(c, 0.0, 1.0), vGammaPower);
}

vec3 gamma_to_linear(vec3 c)
{
	return pow(clamp(c, 0.0, 1.0), vGammaPowerInverse);
}

//===============================================================================================
// SCENE PARAMETERS
//===============================================================================================

// North is pointed by x
// East is pointed by z
// Sea Level : y = 0.0

// TODO : fix misplaced stuff from sun_t to planet_t
	
struct planet_t
{
	float rs;  	// sea level radius
	float ra; 	// atmosphere radius
};

planet_t earth = planet_t(6360.0e3,
						  6420.0e3);

struct sun_t
{  
    vec3 beta_r;	// rayleigh scattering coefs at sea level
	vec3 beta_m; 	// mie scattering coefs at sea level
	float sh_r; 	// rayleigh scale height
	float sh_m; 	// mie scale height
	float i;		// sun intensity
	float mc;	    // mean cosine
	float azi; 		// azimuth
	float alt; 		// altitude
	float ad; 		// angular diameter (between 0.5244 and 5.422 for our sun)
    vec3 color;
};

sun_t sun = sun_t(vec3(5.5e-6, 13.0e-6, 22.1e-6),
				  vec3(21.0e-6),
				  7994.0,
				  1200.0,
				  20.0,
				  0.76,
				  4.4,
				  PI_2,
				  0.53,
				  vec3(1.0, 1.0, 1.0));

// Use www.esrl.noaa.gov/gmd/grad/solcalc/azel.html to get azimuth and altitude
// of our sun at a specific place and time

//===============================================================================================
// RAY MARCHING STUFF
//===============================================================================================

// pred : rd is normalized
bool intersect_with_atmosphere(in vec3 ro, in vec3 rd, in planet_t planet, out float tr)
{
	float c = length(ro); // distance from center of the planet :)
	vec3 up_dir = ro / c;
	float beta = PI - acos(dot(rd, up_dir)); 
	float sb = sin(beta);
	float b = planet.ra;
	float bt = planet.rs - 10.0;
	
	tr = sqrt((b * b) - (c * c) * (sb * sb)) + c * cos(beta); // sinus law
	
	if (sqrt((bt * bt) - (c * c) * (sb * sb)) + c * cos(beta) > 0.0)
		return false;
	
	return true;
}

const int SKYLIGHT_NB_VIEWDIR_SAMPLES = 12;
const int SKYLIGHT_NB_SUNDIR_SAMPLES = 6;

float compute_sun_visibility(in sun_t sun, float alt)
{
	float vap = 0.0;
	float h, a;
	float vvp = clamp((0.5 + alt / sun.ad), 0.0, 1.0); // vertically visible percentage
	if (vvp == 0.0)
		return 0.0;
	else if (vvp == 1.0)
		return 1.0;
		
	bool is_sup;
	
	if (vvp > 0.5)
	{
		is_sup = true;
		h = (vvp - 0.5) * 2.0;
	}
	else
	{
		is_sup = false;
		h = (0.5 - vvp) * 2.0;
	}
	
	float alpha = acos(h) * 2.0;
	a = (alpha - sin(alpha)) / (2.0 * PI);
	
	if (is_sup)
		vap = 1.0 - a;
	else
		vap = a;

	return vap;
}

// pred : rd is normalized
vec3 compute_sky_light(in vec3 ro, in vec3 rd, in planet_t planet, in sun_t sun)
{
    float t1;
	
    if (!intersect_with_atmosphere(ro, rd, planet, t1) || t1 < 0.0)
		return vec3(0.2);
    
    float sl = t1 / float(SKYLIGHT_NB_VIEWDIR_SAMPLES); // seg length
    float t = 0.0;
	
	float calt = cos(sun.alt);
	vec3 sun_dir = vec3(cos(sun.azi) * calt,
						sin(sun.alt),
						sin(sun.azi) * calt);
	/*float sunF = dot(rd,-sun_dir); sunF=.5*sunF+.501; sunF = pow(sunF,400.0);
	sunF += (pow(dot(rd,-sun_dir)*.5+.5,44.0))*.2;
	return vec3(.9,.9,.9)*sunF;*/
	
	
	float mu = dot(rd, sun_dir);
	float mu2 = mu * mu;
	float mc2 = sun.mc * sun.mc;
	
	// rayleigh stuff
	vec3 sumr = vec3(0.0);
    float odr = 0.0; // optical depth
	float phase_r = (3.0 / (16.0 * PI)) * (1.0 + mu2);
	
	// mie stuff
	vec3 summ = vec3(0.0);
	float odm = 0.0; // optical depth
	float phase_m = ((3.0 / (8.0 * PI)) * ((1.0 - mc2) * (1.0 + mu2))) /
		            ((2.0 + mc2) * pow(1.0 + mc2 - 2.0 * sun.mc * mu, 1.5));
    
    for (int i = 0; i < SKYLIGHT_NB_VIEWDIR_SAMPLES; ++i)
	{
		vec3 sp = ro + rd * (t + 0.5 * sl);
        float h = length(sp) - planet.rs;
        float hr = exp(-h / sun.sh_r) * sl;
		odr += hr;
        float hm = exp(-h / sun.sh_m) * sl;
        odm += hm;
		float tm;
		float sp_alt = PI_2 - asin(planet.rs / length(sp));
		sp_alt += acos(normalize(sp).y) + sun.alt;
		float coef = compute_sun_visibility(sun, sp_alt);
		if (intersect_with_atmosphere(sp, sun_dir, planet, tm) || coef > 0.0)
		{
			float sll = tm / float(SKYLIGHT_NB_SUNDIR_SAMPLES);
			float tl = 0.0;
			float odlr = 0.0, odlm = 0.0;
			for (int j = 0; j < SKYLIGHT_NB_SUNDIR_SAMPLES; ++j)
			{
				vec3 spl = sp + sun_dir * (tl + 0.5 * sll);
				float spl_alt = PI_2 - asin(planet.rs / length(spl));
				spl_alt += acos(normalize(spl).y) + sun.alt;
				float coefl = compute_sun_visibility(sun, spl_alt);
				float hl = length(spl) - planet.rs;
				odlr += exp(-hl / sun.sh_r) * sll * (1.0 - log(coefl + 0.000001));
				odlm += exp(-hl / sun.sh_m) * sll * (1.0 - log(coefl + 0.000001));
				tl += sll;
			}
			vec3 tau = sun.beta_r * (odr + odlr) + sun.beta_m * 1.05 * (odm + odlm);
			vec3 attenuation = vec3(exp(-tau.x), exp(-tau.y), exp(-tau.z));
			sumr +=  hr * attenuation * coef;
			summ +=  hm * attenuation * coef;
		}
        t += sl;
    }
	
    return sun.i * (sumr * phase_r * sun.beta_r + summ * phase_m * sun.beta_m);
}

void main(){
	//calculate pixel coordinate
	vec2 q = gl_FragCoord.xy / windSizeAspect.xy;
	vec2 p = -1.0 + 2.0*q;
	p.x *= windSizeAspect.z;//aspect ratio

	//vec3 rd=normalize(p.x * camUU + p.y * camVV + 1.56 * camDir);
	//vec3 gp = camPos + vec3(0.0, earth.rs+ 1.0,0.0);

	//vec3 c_updir    = vec3(0.0, 1.0, 0.0);	
    //vec3 view_dir = camDir;
	
    //vec3 uu = normalize(cross(view_dir, c_updir));
    //vec3 vv = normalize(cross(uu, view_dir));
	//vec3 rd = normalize(p.x * uu + p.y * vv + 1.56 * camDir);
	vec3 rd = normalize(p.x * camUU + p.y * camVV + 1.0 * camDir);//1.0 45º
	vec3 gp = camPos + vec3(0.0, earth.rs + 1.0, 0.0);

	//sun.alt = 2.6;
	sun.alt=sunValues.x;
	sun.azi=sunValues.y;

	/////////////
	/*vec3 c_position = vec3(0.0, 0.1, 1.0);
    vec3 c_lookat   = vec3(0.0, 1.0, 0.0);
	vec3 c_updir    = vec3(0.0, 1.0, 0.0);
    vec3 view_dir = normalize(c_lookat - c_position);
    vec3 uu = normalize(cross(view_dir, c_updir));
    vec3 vv = normalize(cross(uu, view_dir));
	vec3 rd = normalize(p.x * uu + p.y * vv + 1.56 * view_dir);
	vec3 gp = c_position + vec3(0.0, earth.rs + 1.0, 0.0);
	sun.alt = 2.6;
	sun.alt=sunValues.x;
	sun.azi=sunValues.y;*/
	///////////


	vec3 res = compute_sky_light(gp, rd, earth, sun);
	//////////////
	/*if( q.x > 0.45 )
	{
		float crush = 0.1;
		float frange = 12.0;
		float exposure = 128.0;
		res = log2(1.0+res*exposure);
		res = smoothstep(crush, frange, res);
		res = res*res*res*(res*(res*6.0 - 15.0) + 10.0);
	}

	/////////////!!*/
	res = linear_to_gamma(res);
	
	// vignetting
	// tensor product of the parametric curve defined by (4(t-t²))^0.1
	res *= 0.5 + 0.5 * pow(16.0 * q.x * q.y * (1.0 - q.x) * (1.0 - q.y), 0.1);

	outputF=vec4(res,1.0);
	//float v=(outUV.x+outUV.y)/2.0;
	//outputF=vec4(v,v,v,1.0);
}