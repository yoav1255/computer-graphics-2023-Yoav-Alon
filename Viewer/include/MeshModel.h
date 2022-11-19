#pragma once
#include <glm/glm.hpp>
#include <string>
#include "Face.h"
#include <iostream>

class MeshModel
{
public:
	MeshModel(std::vector<Face> faces, std::vector<glm::vec3> vertices, std::vector<glm::vec3> normals, const std::string& model_name);
	virtual ~MeshModel();
	const Face& GetFace(int index) const;
	int GetFacesCount() const;
	const std::string& GetModelName() const;
	friend std::ostream& operator<< (std::ostream& out, MeshModel& mesh);
	int getVerticesCount() const;
	void scale(int scalar);
	void move(int x, int y);
	glm::vec3 GetVertexAt(int index);

	

private:
	std::vector<Face> faces;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::string model_name;
};

