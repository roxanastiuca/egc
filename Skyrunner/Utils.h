#pragma once

#include <string>

// Structure for information regarding one platform:
struct platform {
	static constexpr float width = 2;
	static constexpr float height = 0.1;

	float len;
	glm::vec3 pos; // position
	std::string color = "blue";

	platform() {}
	platform(float _x, float _y, float _z, float _len = 1) : pos(_x, _y, _z), len(_len) {}
	platform(glm::vec3 _pos, float _len = 1) : pos(_pos), len(_len) {}
};