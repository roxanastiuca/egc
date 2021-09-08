#pragma once

#include <string>

#include <include/glm.h>
#include <Core/GPU/Mesh.h>

namespace Object3D
{

	// Create square with given bottom left corner, length and color
	Mesh* CreateFence(glm::vec3 bottomCenter, float length, float height);
}

#pragma once
