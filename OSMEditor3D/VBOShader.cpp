/************************************************************************************************
*
*		@desc Class containing the shader creation
*		@author igarciad
*
************************************************************************************************/

#include "VBOShader.h"
#include <QTextStream>
#include "qstring.h"


	// init static variables
	std::vector<uint> Shader::programs;
	std::vector<uint> Shader::vss;
	std::vector<uint> Shader::fragments;


	void loadTextFile(const QString fileName,std::string& str){
		QFile file(fileName);
		if(!file.open(QIODevice::ReadOnly)) {
			printf("ERROR: loadTexFile: %s\n", file.errorString().toUtf8().constData());
			//QMessageBox::information(0, "error", file.errorString());
		}

		QTextStream in(&file);
		QString text;
		while(!in.atEnd()) {
			QString line = in.readLine();    
			text+=line+"\n"; 
		}
		file.close();
		str = std::string(text.toUtf8().constData());
	}//

	uint loadShader(std::string& source,uint mode){
		GLenum err;
		uint id=glCreateShader(mode);//GL_VERTEX_SHADER,GL_FRAGMENT_SHADER
		const char* csource=source.c_str();
		glShaderSource(id,1,&csource,NULL);
		glCompileShader(id);
		int infologLength = 0;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH,&infologLength);
		if(infologLength>1){
			char *infoLog=(char *)malloc(infologLength);
			glGetShaderInfoLog(id,1000,NULL,infoLog);
			printf("Compile status %d:\n %s\n",infologLength,infoLog);
			free(infoLog);
			exit(1);
		}else{
			printf("Compile status: OK\n");
		}
		//while ((err = glGetError()) != GL_NO_ERROR) qDebug() << "*loadShader ERROR INIT: OpenGL-->" << err << endl;
		return id;
	}//

	uint Shader::initShader(const QString& vertFileName, const QString& fragFileName){
		//GLenum err;
		// read shaders
		std::string source;
		loadTextFile(vertFileName,source);
		uint vs=loadShader(source, GL_VERTEX_SHADER);
		source="";
		loadTextFile(fragFileName,source);
		uint fs=loadShader(source,GL_FRAGMENT_SHADER);
		//create program
		int programId=glCreateProgram();
		glAttachShader(programId,vs);
		glAttachShader(programId,fs);
		glBindFragDataLocation(programId, 0, "outputF");
		glLinkProgram(programId);
		///
		{//check program
			int infologLength = 0;
			glGetProgramiv(programId, GL_INFO_LOG_LENGTH,&infologLength);
			if (infologLength > 0){
				char *infoLog= (char *)malloc(infologLength);
				glGetProgramInfoLog(programId, infologLength, NULL, infoLog);
				printf("%s\n",infoLog);
				free(infoLog);
			}
		}
		//while ((err = glGetError()) != GL_NO_ERROR) qDebug() << "*initShader ERROR INIT: OpenGL-->" << err << endl;
		VBOUtil::check_gl_error("InitShader");
		///
		//programIdVectorIndex[programId]=programs.size();
		//nameToLocation.push_back(QHash<QString,GLint>());

		programs.push_back(programId);
		vss.push_back(vs);
		fragments.push_back(fs);
		return programId;
	}//

	void Shader::cleanShaders(){
		for(int pN=0;pN<programs.size();pN++){
			glDetachShader(programs[pN],vss[pN]);
			glDetachShader(programs[pN],fragments[pN]);
			glDeleteShader(vss[pN]);
			glDeleteShader(fragments[pN]);
			glDeleteProgram(programs[pN]);
		}
		programs.clear();
		vss.clear();
		fragments.clear();
	}//
	



