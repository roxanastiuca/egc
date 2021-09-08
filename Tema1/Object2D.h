#pragma once

#include <string>

#include <include/glm.h>
#include <Core/GPU/Mesh.h>

/*
	Class used for drawing the meshes necessary in the game: bow, arrow, shuriken, balloon (and string of balloon),
	heart (for keeping track of lives left), rectangles (for score bar and arrow power bar) and stick figure (archer).
*/
namespace Object2D
{
	Mesh* CreateBow(std::string name, glm::vec3 center, float radius, float thickness, glm::vec3 color);
	Mesh* CreateArrow(std::string name, glm::vec3 leftCorner, float length, float thickness, glm::vec3 color);
	Mesh* CreateShuriken(std::string name, glm::vec3 center, float side, glm::vec3 color);
	Mesh* CreateBalloon(std::string name, glm::vec3 bottomCenter, float width, float height, float neckLength, glm::vec3 color);
	Mesh* CreateBalloonString(std::string name, glm::vec3 balloonBottomCenter, float height, glm::vec3 color);
	Mesh* CreateRectangle(std::string name, glm::vec3 bottomCenter, float width, float height, glm::vec3 color);
	Mesh* CreateHeart(std::string name, glm::vec3 bottom, float width, float height, glm::vec3 color);
	Mesh* CreateStickFigure(std::string name, glm::vec3 origin, float height, glm::vec3 color);
}

