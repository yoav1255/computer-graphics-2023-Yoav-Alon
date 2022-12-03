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
	void Ortho(float left, float right, float bottom, float top, float znear, float zfar) { projection_transformation = glm::ortho(left, right, bottom, top, znear, zfar); }
	void Frustum();
	void Perspective(float fovy, float aspect, float znear, float zfar) { projection_transformation = glm::perspective(fovy,aspect,znear,zfar); } // aspect =width/height
	
	const glm::mat4x4& GetProjectionTransformation() const;
	const glm::mat4x4& GetViewTransformation() const;

private:
	glm::mat4x4 view_transformation;
	glm::mat4x4 projection_transformation;
};
