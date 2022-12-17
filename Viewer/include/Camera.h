#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
class Camera
{
public:
	Camera();
	virtual ~Camera();

	void SetTransformation(const glm::mat4x4& transform);
	void SetCameraLookAt(const glm::vec3& eye, const glm::vec3& at, const glm::vec3& up);
	void SetProjection(const glm::mat4x4& projection);
	void Ortho(float left, float right, float bottom, float top, float znear, float zfar);
	void Perspective(float aspect);

	
	const glm::mat4x4& GetProjectionTransformation() const;
	const glm::mat4x4& GetViewTransformation() const;

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

	const glm::vec3& GetTranslationWorld() const { return translationWorld; }
	const glm::vec3& GetScaleWorld() const { return scaleWorld; }
	const glm::vec3& GetRotationWorld() const { return rotationWorld; }
	void setDolly(float distance);
	void SetTranslationWorld(const glm::vec3 translationVec) { translationWorld = translationVec; }
	void SetScaleWorld(const glm::vec3 scaleVec) { scaleWorld = scaleVec; }
	void SetRotationWorld(const glm::vec3 rotationVec) { rotationWorld = rotationVec; }
	float dolly;
	float fov;
	float zNearPerspective;
	float zFarPerspective;
	glm::vec3 translationWorld;	
	glm::vec3 translationObject;


private:
	glm::mat4x4 view_transformation;
	glm::mat4x4 projection_transformation;

	glm::mat4 objectTransform;
	glm::mat4 worldTransform;
	glm::vec3 scaleObject;
	glm::vec3 rotationObject;
	
	glm::vec3 scaleWorld;
	glm::vec3 rotationWorld;
};
