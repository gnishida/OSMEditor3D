#include "PolygonOffset.h"
#include <algorithm>

namespace polyoffset {
	const double M_PI = 3.141592653589793238462643383279;

	bool sort_pred1(const std::pair<double, std::pair<int, glm::dvec2>>& left, const std::pair<double, std::pair<int, glm::dvec2>>& right) {
		return left.first < right.first;
	}

	bool sort_pred2(const std::pair<double, std::pair<int, glm::dvec2>>& left, const std::pair<double, std::pair<int, glm::dvec2>>& right) {
		return left.second.first < right.second.first;
	}

	void polygonOffset(const std::vector<glm::vec2>& polygon, const std::vector<float>& offsetDistance, std::vector<std::vector<glm::vec2>>& offsetPolygons) {
		std::vector<glm::dvec2> rawInterPolygon;
		std::vector<bool> originalVertices;
		for (int i = 0; i < polygon.size(); ++i) {
			glm::dvec2 prev = polygon[(i - 1 + polygon.size()) % polygon.size()];
			glm::dvec2 cur = polygon[i];
			glm::dvec2 next = polygon[(i + 1) % polygon.size()];

			glm::dvec2 dir = next - cur;
			dir /= glm::length(dir);
			glm::dvec2 prev_dir = cur - prev;
			prev_dir /= glm::length(prev_dir);

			// check whether the current vertex is convex or concave
			bool convex_vertex = true;
			if (prev_dir.x * dir.y - prev_dir.y * dir.x <= 0) {
				convex_vertex = false;
			}

			// offset
			double offset = offsetDistance[i];
			double prev_offset = offsetDistance[(i - 1 + offsetDistance.size()) % offsetDistance.size()];

			// compute the normal vector that is outerward from the region
			glm::dvec2 n(dir.y, -dir.x);
			glm::dvec2 prev_n(prev_dir.y, -prev_dir.x);

			// compute the offset vertices
			glm::dvec2 p0 = cur - prev_n * prev_offset;
			glm::dvec2 p1 = cur - n * offset;
			glm::dvec2 p2 = next - n * offset;

			if (glm::length(p0 - p1) < 0.1) {
				rawInterPolygon.push_back(p2);
				originalVertices.push_back(false);
			}
			else if (convex_vertex) {
				rawInterPolygon.push_back(cur);
				originalVertices.push_back(true);
				rawInterPolygon.push_back(p1);
				originalVertices.push_back(true);
				rawInterPolygon.push_back(p2);
				originalVertices.push_back(true);
			}
			else {
				// arc
				double theta1 = atan2(p0.y - cur.y, p0.x - cur.x);
				double theta2 = atan2(p1.y - cur.y, p1.x - cur.x);
				if (theta2 >= theta1) theta2 -= M_PI * 2.0f;
				for (double theta = theta1 - 0.2f; theta >= theta2 + 0.2; theta -= 0.2) {
					glm::vec2 p = cur + glm::dvec2(cos(theta) * offset, sin(theta) * offset);
					rawInterPolygon.push_back(p);
					originalVertices.push_back(false);
				}

				rawInterPolygon.push_back(p1);
				originalVertices.push_back(false);
				rawInterPolygon.push_back(p2);
				originalVertices.push_back(false);
			}
		}

		for (int i = 0; i < rawInterPolygon.size();) {
			int i2 = (i + 1) % rawInterPolygon.size();

			std::vector<std::pair<double, std::pair<int, glm::dvec2>>> intersections;
			/*std::vector<int> intersected_ids;
			std::vector<glm::vec2> intPoints;*/
			for (int j = i + 1; j < rawInterPolygon.size(); ++j) {
				int j2 = (j + 1) % rawInterPolygon.size();

				double tab, tcd;
				glm::dvec2 intPoint;
				if (segmentSegmentIntersect(rawInterPolygon[i], rawInterPolygon[i2], rawInterPolygon[j], rawInterPolygon[j2], &tab, &tcd, true, intPoint)) {
					intersections.push_back(std::make_pair(tab, std::make_pair(j, intPoint)));
					/*
					intersected_ids.push_back(j);
					intPoints.push_back(intPoint);
					*/
				}
			}

			if (intersections.size() > 0) {
				std::sort(intersections.begin(), intersections.end(), sort_pred1);
				for (int j = 0; j < intersections.size(); ++j) {
					rawInterPolygon.insert(rawInterPolygon.begin() + i + j + 1, intersections[j].second.second);
					originalVertices.insert(originalVertices.begin() + i + j + 1, false);
				}
				std::sort(intersections.begin(), intersections.end(), sort_pred2);
				for (int j = 0; j < intersections.size(); ++j) {
					rawInterPolygon.insert(rawInterPolygon.begin() + intersections.size() + intersections[j].second.first + j + 1, intersections[j].second.second);
					originalVertices.insert(originalVertices.begin() + intersections.size() + intersections[j].second.first + j + 1, false);
				}
				i += intersections.size() + 1;
			}
			else {
				i++;
			}
		}

		std::vector<std::vector<glm::dvec2>> partitions;
		for (int i = 0; i < rawInterPolygon.size(); ++i) {
			if (originalVertices[i]) continue;

			for (int j = i - 1; j >= 0; --j) {
				if (rawInterPolygon[j] == rawInterPolygon[i]) {
					partitions.push_back(std::vector<glm::dvec2>());
					for (int k = j; k < i; ++k) {
						partitions.back().push_back(rawInterPolygon[k]);
					}
					for (int k = i - 1; k >= j; --k) {
						rawInterPolygon.erase(rawInterPolygon.begin() + k);
						originalVertices.erase(originalVertices.begin() + k);
					}

					i -= (i - j);
					break;
				}
			}
		}
		if (rawInterPolygon.size() > 0) {
			partitions.push_back(std::vector<glm::dvec2>());
			for (int i = 0; i < rawInterPolygon.size(); ++i) {
				partitions.back().push_back(rawInterPolygon[i]);
			}
		}

		for (int i = 0; i < partitions.size(); ++i) {
			if (!isClockwise(partitions[i])) {
				offsetPolygons.push_back(std::vector<glm::vec2>());
				for (int k = 0; k < partitions[i].size(); ++k) {
					offsetPolygons.back().push_back(glm::vec2(partitions[i][k]));
				}
			}
		}
	}

	bool segmentSegmentIntersect(const glm::dvec2& a, const glm::dvec2& b, const glm::dvec2& c, const glm::dvec2& d, double *tab, double *tcd, bool segmentOnly, glm::dvec2& intPoint) {
		glm::dvec2 u = b - a;
		glm::dvec2 v = d - c;

		if (glm::length(u) == 0.0 || glm::length(v) == 0.0) {
			return false;
		}

		double numer = v.x*(c.y - a.y) + v.y*(a.x - c.x);
		double denom = u.y*v.x - u.x*v.y;

		if (denom == 0.0)  {
			// they are parallel
			*tab = 0.0;
			*tcd = 0.0;
			return false;
		}

		double t0 = numer / denom;

		glm::dvec2 ipt = a + t0*u;
		glm::dvec2 tmp = ipt - c;
		double t1;
		if (tmp.x * v.x + tmp.y * v.y > 0.0f) {
			t1 = glm::length(tmp) / glm::length(v);
		}
		else {
			t1 = -1.0 * glm::length(tmp) / glm::length(v);
		}

		//Check if intersection is within segments
		//if (segmentOnly && !((t0 >= 0.0000001f) && (t0 <= 1.0f - 0.0000001f) && (t1 >= 0.0000001f) && (t1 <= 1.0f - 0.0000001f))) {
		if (segmentOnly && !(t0 > 0.0 && t0 < 1.0 && t1 > 0.0 && t1 < 1.0)) {
			return false;
		}

		*tab = t0;
		*tcd = t1;
		glm::dvec2 dirVec = b - a;

		intPoint = a + (*tab) * dirVec;

		return true;
	}

	bool isClockwise(const std::vector<glm::dvec2>& polygon) {
		double tmpSum = 0.0f;

		for (int i = 0; i < polygon.size(); ++i) {
			int next = (i + 1) % polygon.size();
			tmpSum = tmpSum + (polygon[next].x - polygon[i].x) * (polygon[next].y + polygon[i].y);
		}

		if (tmpSum > 0.0) {
			return true;
		}
		else {
			return false;
		}
	}

}