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
	const std::vector<glm::vec3> GetVertices() const;
	int GetFacesCount() const;
	const std::string& GetModelName() const;
	friend std::ostream& operator<<(std::ostream &os, const std::shared_ptr<MeshModel> &myModel);

private:
	std::vector<Face> faces;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::string model_name;
};
