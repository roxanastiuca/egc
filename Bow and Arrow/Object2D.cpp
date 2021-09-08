#include "Object2D.h"

#include <Core/Engine.h>

/**
	* Creare bow using GL_TRIANGLE_STRIP.
	* First, create thick semi-circle: for each angle place one vertex on the circle described by (center, radius) and one vertex
	on the circle described by (center, radius+thickness).
	* Then, draw bow handle as a rectangle.
**/
Mesh* Object2D::CreateBow(std::string name, glm::vec3 center, float radius, float thickness, glm::vec3 color) {
	Mesh* bow = new Mesh(name);
	bow->SetDrawMode(GL_TRIANGLE_STRIP);

	std::vector<VertexFormat>vertices;
	std::vector<unsigned short> indices;

	int index = 0;

	// Bow semi-circle, between [-90, 90] degrees:
	for (int i = -90; i <= 90; i++) {
		float radians = i * (float)M_PI / 180; // convert to radians

		vertices.push_back(VertexFormat(center + glm::vec3(cos(radians) * radius, sin(radians) * radius, 0), color));
		vertices.push_back(VertexFormat(center + glm::vec3(cos(radians) * (radius + thickness), sin(radians) * (radius + thickness), 0), color));
		indices.push_back(index++);
		indices.push_back(index++);
	}

	// Bow handle:
	vertices.push_back(VertexFormat(center + glm::vec3(0, radius + thickness, 0), color));
	vertices.push_back(VertexFormat(center + glm::vec3(-thickness, radius + thickness, 0), color));
	vertices.push_back(VertexFormat(center + glm::vec3(0, - radius - thickness, 0), color));
	vertices.push_back(VertexFormat(center + glm::vec3(-thickness, - radius - thickness, 0), color));
	for (int i = 0; i < 4; i++)	indices.push_back(index++);

	bow->InitFromData(vertices, indices);
	return bow;
}

/**
	* Create arrow, using GL_TRIANGLES.
	* First, create an rectangle of sides (length, thickness), using 2 triangles.
	* Then, create arrowhead by drawing one triangle on the rightmost edge.
**/
Mesh* Object2D::CreateArrow(std::string name, glm::vec3 leftCorner, float length, float thickness, glm::vec3 color) {
	Mesh* arrow = new Mesh(name);
	arrow->SetDrawMode(GL_TRIANGLES);

	glm::vec3 corner = leftCorner;
	glm::vec3 center = leftCorner + glm::vec3(0, thickness / 2, 0);

	std::vector<VertexFormat> vertices;
	std::vector<unsigned short> indices;

	vertices.push_back(VertexFormat(corner, color));
	vertices.push_back(VertexFormat(corner + glm::vec3(length, 0, 0), color));
	vertices.push_back(VertexFormat(corner + glm::vec3(0, thickness, 0), color));
	vertices.push_back(VertexFormat(corner + glm::vec3(length, thickness, 0), color));

	// add triangle in the rightmost edge
	float peak_thickness = thickness * 2;

	vertices.push_back(VertexFormat(center + glm::vec3(length + peak_thickness, 0, 0), color));
	vertices.push_back(VertexFormat(center + glm::vec3(length, -peak_thickness, 0), color));
	vertices.push_back(VertexFormat(center + glm::vec3(length, peak_thickness, 0), color));
	
	indices = {
		0, 1, 2,
		2, 1, 3,
		4, 5, 6
	};

	arrow->InitFromData(vertices, indices);
	return arrow;
}

/**
	* Create shuriken, using 4 triangles, all intersecting in center.
**/
Mesh* Object2D::CreateShuriken(std::string name, glm::vec3 center, float side, glm::vec3 color) {
	Mesh* shuriken = new Mesh(name);
	shuriken->SetDrawMode(GL_TRIANGLES);

	std::vector<VertexFormat> vertices;
	std::vector<unsigned short> indices;

	vertices.push_back(VertexFormat(center, color));
	vertices.push_back(VertexFormat(center + glm::vec3(0, side, 0), color));
	vertices.push_back(VertexFormat(center + glm::vec3(-side, side, 0), color));
	vertices.push_back(VertexFormat(center + glm::vec3(-side, 0, 0), color));
	vertices.push_back(VertexFormat(center + glm::vec3(-side, -side, 0), color));
	vertices.push_back(VertexFormat(center + glm::vec3(0, -side, 0), color));
	vertices.push_back(VertexFormat(center + glm::vec3(side, -side, 0), color));
	vertices.push_back(VertexFormat(center + glm::vec3(side, 0, 0), color));
	vertices.push_back(VertexFormat(center + glm::vec3(side, side, 0), color));

	indices = {
		0, 1, 2,
		0, 3, 4,
		0, 5, 6,
		0, 7, 8
	};

	shuriken->InitFromData(vertices, indices);
	return shuriken;
}

/**
	* Draw balloon, using GL_TRIANGLE_FAN.
	* Place v0 (center of fan) slightly above the bottom of the ellipse, so that we can start a triangle from it (which is the neck
	of the balloon).
	* Draw ellipse. Then draw triangle on the bottom.
**/
Mesh* Object2D::CreateBalloon(std::string name, glm::vec3 bottomCenter, float width, float height, float neckLength, glm::vec3 color) {
	Mesh* balloon = new Mesh(name);
	balloon->SetDrawMode(GL_TRIANGLE_FAN);

	std::vector<VertexFormat> vertices;
	std::vector<unsigned short> indices;

	float radiusOX = width / 2;
	float radiusOY = height / 2;

	glm::vec3 center = bottomCenter + glm::vec3(0, radiusOY, 0);

	// v0:
	vertices.push_back(VertexFormat(bottomCenter + glm::vec3(0, neckLength / 4, 0), color));
	indices.push_back(0);

	// Draw ellipse:
	for (int i = 0; i <= 360; i++) {
		float radians = i * (float)M_PI / 180;

		vertices.push_back(VertexFormat(center + glm::vec3(cos(radians) * radiusOX, sin(radians) * radiusOY, 0), color));
		indices.push_back(i + 1);
	}

	// Draw neck:
	vertices.push_back(VertexFormat(bottomCenter + glm::vec3(- neckLength, - neckLength / 2, 0), color));
	vertices.push_back(VertexFormat(bottomCenter + glm::vec3(neckLength, - neckLength / 2, 0), color));
	indices.push_back(362);
	indices.push_back(363);

	balloon->InitFromData(vertices, indices);
	return balloon;
}

/**
	* Draw the string of a balloon, using GL_LINE_STRIP.
	* It has a Zig Zag shape.
**/
Mesh* Object2D::CreateBalloonString(std::string name, glm::vec3 balloonBottomCenter, float height, glm::vec3 color) {
	Mesh* string = new Mesh(name);
	string->SetDrawMode(GL_LINE_STRIP);

	glm::vec3 start = balloonBottomCenter;

	std::vector<VertexFormat> vertices = {
		VertexFormat(start, color),
		VertexFormat(start + glm::vec3(-height / 5 - 1, - height / 4, 0), color),
		VertexFormat(start + glm::vec3(height / 5 + 1, - height / 2, 0), color),
		VertexFormat(start + glm::vec3(- height / 5 + 1, - 3 * height / 4, 0), color),
		VertexFormat(start + glm::vec3(height / 5 - 1, -height, 0), color)
	};
	std::vector<unsigned short> indices = {0, 1, 2, 3, 4};

	string->InitFromData(vertices, indices);
	return string;
}

/**
	* Draw a rectangle, using 2 triangles.
**/
Mesh* Object2D::CreateRectangle(std::string name, glm::vec3 bottomCenter, float width, float height, glm::vec3 color)
{

	std::vector<VertexFormat> vertices =
	{
		VertexFormat(bottomCenter + glm::vec3(-width / 2, 0, 0), color),
		VertexFormat(bottomCenter + glm::vec3(width / 2, 0, 0), color),
		VertexFormat(bottomCenter + glm::vec3(width / 2, height, 0), color),
		VertexFormat(bottomCenter + glm::vec3(-width / 2, height, 0), color)
	};

	Mesh* square = new Mesh(name);
	std::vector<unsigned short> indices = {
		0, 1, 2,
		0, 2, 3
	};

	square->SetDrawMode(GL_TRIANGLES);
	square->InitFromData(vertices, indices);
	return square;
}

/**
	* Draw a heart, using GL_TRIANGLE_FAN.
	* v0 (center of fan) is the lowermost point.
	* A heart has the following components:
		- straight line from v0 to rightmost point.
		- semi-circle between [-45, 135] degrees, starting from rightmost point.
		- 2 staright lines between the 2 semi-circles that connect, forming a V shape.
		- semi-circle between [45, 225] degrees, ending on the leftmost point.
		- straight line from leftmost point to v0.
**/
Mesh* Object2D::CreateHeart(std::string name, glm::vec3 bottom, float width, float height, glm::vec3 color) {
	Mesh *heart = new Mesh(name);

	std::vector<VertexFormat> vertices;
	std::vector<unsigned short> indices;

	unsigned short index = 0;

	vertices.push_back(VertexFormat(bottom, color));
	indices.push_back(index++);

	float radius = width / 5; // radius of both semi-circles
	glm::vec3 center1 = bottom + glm::vec3(radius * 3/2, height * 3/4, 0); // center of first semi-circle
	glm::vec3 center2 = bottom + glm::vec3(-radius * 3/2, height * 3/4, 0); // center of second semi-circle

	// Define first semi-circles points that connect to v0.
	for (float i = -45; i <= 135; i++) {
		float radians = i * (float)M_PI / 180;
		vertices.push_back(VertexFormat(center1 + glm::vec3(radius * cos(radians), radius * sin(radians), 0), color));
		indices.push_back(index++);
	}

	float x = center1[0] + radius * cos(M_PI / 2 + M_PI / 4);
	float y = center1[1] + radius * sin(M_PI / 2 + M_PI / 4);

	// Draw the right side of the V connecting the 2 semi-circles.
	while (y >= height * 3 / 4) {
		x -= 0.01;
		y -= 0.01;
		vertices.push_back(VertexFormat(glm::vec3(x, y, 0), color));
		indices.push_back(index++);
	}

	// Draw the left side of the V connecting the 2 semi-circles.
	float y_max = center2[1] + radius * sin(M_PI / 4);
	while (y <= y_max) {
		x -= 0.01;
		y += 0.01;
		vertices.push_back(VertexFormat(glm::vec3(x, y, 0), color));
		indices.push_back(index++);
	}

	// Define second semi-circles points that connect to v0.
	for (float i = 45; i <= 225; i++) {
		float radians = i * (float)M_PI / 180;
		vertices.push_back(VertexFormat(center2 + glm::vec3(radius * cos(radians), radius * sin(radians), 0), color));
		indices.push_back(index++);
	}

	heart->SetDrawMode(GL_TRIANGLE_FAN);
	heart->InitFromData(vertices, indices);
	return heart;
}

/**
	* Draw a stick figure, using lines.
	* The figure has 6 straight lines (2 hands, 2 legs, torso, neck) and a circle for its face (drawn with really small lines).
**/
Mesh* Object2D::CreateStickFigure(std::string name, glm::vec3 origin, float height, glm::vec3 color) {
	Mesh *figure = new Mesh(name);
	figure->SetDrawMode(GL_LINES);

	float unit = height / 6;

	std::vector<VertexFormat> vertices{
		VertexFormat(origin, color),
		VertexFormat(origin + glm::vec3(-unit, 0, 0), color),
		VertexFormat(origin + glm::vec3(-unit, unit, 0), color),
		VertexFormat(origin + glm::vec3(-2 * unit, -unit, 0), color),
		VertexFormat(origin + glm::vec3(-unit, -unit, 0), color),
		VertexFormat(origin + glm::vec3(-2 * unit, -3 * unit, 0), color),
		VertexFormat(origin + glm::vec3(0, -3 * unit, 0), color)
	};

	std::vector<unsigned short> indices{
		0, 1,
		1, 3,
		1, 4,
		4, 5,
		4, 6,
		1, 2
	};

	glm::vec3 c = origin + glm::vec3(-unit, 2 *  unit, 0);
	int index = 7;

	for (int i = 0; i < 360; i++) {
		float radians = i * (float)M_PI / 180;
		vertices.push_back(VertexFormat(c + glm::vec3(unit * cos(radians), unit * sin(radians), 0), color));
		indices.push_back(index);
		indices.push_back(++index);
	}
	vertices.push_back(VertexFormat(origin + glm::vec3(0, 2 * unit, 0), color));

	figure->InitFromData(vertices, indices);
	return figure;
}