#pragma once
#include <glm/glm.hpp>
#include <string>
#include "Face.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

class MeshModel
{
public:
	MeshModel(std::vector<Face> faces, std::vector<glm::vec3> vertices, std::vector<glm::vec3> normals, const std::string& model_name);
	virtual ~MeshModel();
	const Face& GetFace(int index) const;
	std::vector<glm::vec3> GetVertices() const;
	void setVertices();
	int GetFacesCount() const;
	const std::string& GetModelName() const;
	friend std::ostream& operator<<(std::ostream &os, const std::shared_ptr<MeshModel> &myModel);
	 glm::mat4x4 GetTransform()const;
	 glm::mat4x4 GetObjectTransform();
	 glm::mat4x4 GetWorldTransform();
	void SetObjectTransform();
	void SetWorldTransform(const glm::mat4& mat);

	const glm::vec3& GetTranslation() const { return translation; }
	const glm::vec3& GetScale() const { return scale; }
	const glm::vec3& GetRotation() const { return rotation; }

	void SetTranslation(const glm::vec3 translationVec) { translation = translationVec; }
	void SetScale(const glm::vec3 scaleVec) { scale = scaleVec; }
	void SetRotation(const glm::vec3 rotationVec) { rotation = rotationVec; }

	

private:
	std::vector<Face> faces;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	glm::mat4 objectTransform;
	glm::mat4 worldTransform;
	glm::vec3 translation;
	glm::vec3 scale;
	glm::vec3 rotation;
	std::string model_name;
};
