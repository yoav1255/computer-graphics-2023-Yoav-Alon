#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

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
	glm::vec3 GetVertexAt(int index);
	glm::vec3 scaleLocal;
	glm::vec3 translateLocal;
	glm::vec3 rotateLocal;
	glm::vec3 scaleWorld;
	glm::vec3 translateWorld;
	glm::vec3 rotateWorld;
	glm::mat4 scaleMat;
	glm::mat4 rotateMat;
	glm::mat4 translateMat;
	glm::mat4 getLocal();
	void setLocal();
	glm::mat4 getWorld();
	void setWorld();



	

private:
	std::vector<Face> faces;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::string model_name;
	glm::mat4 local;
	glm::mat4 world;
};

