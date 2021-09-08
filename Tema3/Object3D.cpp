#include "Object3D.h"

#include <Core/Engine.h>

void DrawRectangle(glm::vec3 corner, float side, float height, unsigned short index,
	std::vector<VertexFormat> vertices, std::vector<unsigned short> indices) {
	
	vertices.push_back(VertexFormat(corner + glm::vec3(0, 0, side), glm::vec3(0, 0, 0)));
	vertices.push_back(VertexFormat(corner + glm::vec3(side, 0, side), glm::vec3(0, 0, 0)));
	vertices.push_back(VertexFormat(corner + glm::vec3(0, height, side), glm::vec3(0, 0, 0)));
	vertices.push_back(VertexFormat(corner + glm::vec3(side, height, side), glm::vec3(0, 0, 0)));
	vertices.push_back(VertexFormat(corner + glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)));
	vertices.push_back(VertexFormat(corner + glm::vec3(side, 0, 0), glm::vec3(0, 0, 0)));
	vertices.push_back(VertexFormat(corner + glm::vec3(0, height, 0), glm::vec3(0, 0, 0)));
	vertices.push_back(VertexFormat(corner + glm::vec3(side, height, 0), glm::vec3(0, 0, 0)));

	indices.push_back(index); indices.push_back(index + 1); indices.push_back(index + 2);
	indices.push_back(index + 1); indices.push_back(index + 3); indices.push_back(index + 2);
	indices.push_back(index + 2); indices.push_back(index + 3); indices.push_back(index + 7);
	indices.push_back(index + 2); indices.push_back(index + 7); indices.push_back(index + 6);
	indices.push_back(index + 1); indices.push_back(index + 7); indices.push_back(index + 3);
	indices.push_back(index + 1); indices.push_back(index + 5); indices.push_back(index + 7);
	indices.push_back(index + 6); indices.push_back(index + 7); indices.push_back(index + 4);
	indices.push_back(index + 7); indices.push_back(index + 5); indices.push_back(index + 4);
	indices.push_back(index + 0); indices.push_back(index + 4); indices.push_back(index + 1);
	indices.push_back(index + 1); indices.push_back(index + 4); indices.push_back(index + 5);
	indices.push_back(index + 2); indices.push_back(index + 6); indices.push_back(index + 4);
	indices.push_back(index + 0); indices.push_back(index + 2); indices.push_back(index + 4);

}

Mesh* Object3D::CreateFence(glm::vec3 bottomCenter, float length, float height) {
	Mesh* fence = new Mesh("fence");
	fence->SetDrawMode(GL_TRIANGLES);

	std::vector<VertexFormat> vertices;
	std::vector<unsigned short> indices;

	DrawRectangle(bottomCenter + glm::vec3(0, 0, length / 2), 1, height, 0, vertices, indices);
	DrawRectangle(bottomCenter - glm::vec3(0, 0, - length / 2), 1, height, 0, vertices, indices);
	DrawRectangle(bottomCenter)

	fence->InitFromData(vertices, indices);
	return fence;
}
}