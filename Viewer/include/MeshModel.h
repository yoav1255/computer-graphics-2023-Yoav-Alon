#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <memory>
#include "MeshModel.h"
#include "Face.h"


struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 textureCoords;
};

class MeshModel
{
public:
	std::vector<Face> faces;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec3> textureCoords;

	std::vector<Vertex> modelVertices;

	glm::mat4 objectTransform;
	glm::mat4 worldTransform;
	glm::vec3 vCenter;
	glm::vec3 translationObject;
	glm::vec3 scaleObject;
	glm::vec3 rotationObject;
	glm::vec3 translationWorld;
	glm::vec3 scaleWorld;
	glm::vec3 rotationWorld;

	std::string modelName;

	glm::vec3 color;

	GLuint vbo;
	GLuint vao;


	MeshModel(std::vector<Face> faces, std::vector<glm::vec3> vertices, std::vector<glm::vec3> normals, std::vector<glm::vec2> textureCoords, const std::string& modelName = "");	virtual ~MeshModel();
	const Face& GetFace(int index) const;
	std::vector<glm::vec3> GetVertices() const;
	void setVertices(const glm::mat4 mat);
	int GetFacesCount() const;
	const std::string& GetModelName() const;
	friend std::ostream& operator<<(std::ostream &os, const std::shared_ptr<MeshModel> &myModel);
	glm::mat4	 GetTransform();
	 glm::mat4 GetObjectTransform();
	 glm::mat4 GetWorldTransform();
	void SetObjectTransform();
	void SetWorldTransform();
	std::vector<glm::vec3> meshColors;
	const std::vector<Vertex>& GetModelVertices();
	void TranslateModel(const glm::vec3& translationVector);
	void TranslateWorld(const glm::vec3& translationVector);

	void RotateXModel(double angle);
	void RotateYModel(double angle);
	void RotateZModel(double angle);
	void ScaleXModel(double factor);
	void ScaleYModel(double factor);
	void ScaleZModel(double factor);
	void ScaleModel(double factor);

	void RotateXWorld(double angle);
	void RotateYWorld(double angle);
	void RotateZWorld(double angle);
	void ScaleXWorld(double factor);
	void ScaleYWorld(double factor);
	void ScaleZWorld(double factor);
	void ScaleWorld(double factor);

	const glm::vec3& GetTranslationObject() const { return translationObject; }
	const glm::vec3& GetScaleObject() const { return scaleObject; }
	const glm::vec3& GetRotationObject() const { return rotationObject; }

	const glm::mat4 GetRotationObjectMat() const;
	const glm::mat4 GetTranslationObjectMat() const;
	const glm::mat4 GetScaleObjectMat() const;

	const glm::mat4 GetRotationWorldMat() const;
	const glm::mat4 GetTranslationWorldMat() const;
	const glm::mat4 GetScaleWorldMat() const;

	void SetTranslationObject(const glm::vec3 translationVec) { translationObject = translationVec; }
	void SetScaleObject(const glm::vec3 scaleVec) { scaleObject = scaleVec; }
	void SetRotationObject(const glm::vec3 rotationVec) { rotationObject = rotationVec; }
	void SetVCenter(const glm::vec3 center) { vCenter = center; }

	const glm::vec3& GetTranslationWorld() const { return translationWorld; }
	const glm::vec3& GetScaleWorld() const { return scaleWorld; }
	const glm::vec3& GetRotationWorld() const { return rotationWorld; }

	const glm::vec3& GetVCenter() const { return vCenter; }

	
	void SetTranslationWorld(const glm::vec3 translationVec) { translationWorld = translationVec; }
	void SetScaleWorld(const glm::vec3 scaleVec) { scaleWorld = scaleVec; }
	void SetRotationWorld(const glm::vec3 rotationVec) { rotationWorld = rotationVec; }

	void initNormals();
	const glm::vec3& GetColor() const;
	void SetColor(const glm::vec3& color);
	
	GLuint GetVAO() const;

	
	bool axisLocal;
	bool axisWorld;
	bool bounding_box_local;
	bool bounding_box_world;
	bool drawVerticeNormals;
	bool drawFaceNormals;
	bool drawRectangle;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

};
