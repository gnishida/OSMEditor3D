#pragma once

#include <vector>
#include <glm/glm.hpp>

namespace polyoffset {
	void polygonOffset(const std::vector<glm::vec2>& polygon, const std::vector<float>& offsetDistance, std::vector<std::vector<glm::vec2>>& offsetPolygons);
	bool segmentSegmentIntersect(const glm::dvec2& a, const glm::dvec2& b, const glm::dvec2& c, const glm::dvec2& d, double *tab, double *tcd, bool segmentOnly, glm::dvec2& intPoint);
	bool isClockwise(const std::vector<glm::dvec2>& polygon);
}