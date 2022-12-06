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
	void setVertices(const glm::mat4 mat);
	int GetFacesCount() const;
	const std::string& GetModelName() const;
	friend std::ostream& operator<<(std::ostream &os, const std::shared_ptr<MeshModel> &myModel);
	 glm::mat4x4 GetTransform();
	 glm::mat4x4 GetObjectTransform();
	 glm::mat4x4 GetWorldTransform();
	void SetObjectTransform();
	void SetWorldTransform();

	const glm::vec3& GetTranslationObject() const { return translationObject; }
	const glm::vec3& GetScaleObject() const { return scaleObject; }
	const glm::vec3& GetRotationObject() const { return rotationObject; }

	void SetTranslationObject(const glm::vec3 translationVec) { translationObject = translationVec; }
	void SetScaleObject(const glm::vec3 scaleVec) { scaleObject = scaleVec; }
	void SetRotationObject(const glm::vec3 rotationVec) { rotationObject = rotationVec; }
	void SetVCenter(const glm::vec3 center) { vCenter = center; }


	const glm::vec3& GetTranslationWorld() const { return translationWorld; }
	const glm::vec3& GetScaleWorld() const { return scaleWorld; }
	const glm::vec3& GetRotationWorld() const { return rotationWorld; }
	const glm::vec3& GetVCenter() const { return vCenter; }

	bool getAxisLocal() { return axisLocal; }
	bool getAxisWorld() { return axisWorld; }
	bool getbBoxLocal() { return bounding_box_local; }
	bool getbBoxWorld() { return bounding_box_world; }


	void SetTranslationWorld(const glm::vec3 translationVec) { translationWorld = translationVec; }
	void SetScaleWorld(const glm::vec3 scaleVec) { scaleWorld = scaleVec; }
	void SetRotationWorld(const glm::vec3 rotationVec) { rotationWorld = rotationVec; }

	
	std::vector<Face> faces;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	glm::mat4 objectTransform;
	glm::mat4 worldTransform;
	glm::vec3 vCenter;
	glm::vec3 translationObject;
	glm::vec3 scaleObject;
	glm::vec3 rotationObject;
	glm::vec3 translationWorld;
	glm::vec3 scaleWorld;
	glm::vec3 rotationWorld;
	std::string model_name;
	bool axisLocal;
	bool axisWorld;
	bool bounding_box_local;
	bool bounding_box_world;
	bool drawVerticeNormals;
	bool drawFaceNormals;
};
