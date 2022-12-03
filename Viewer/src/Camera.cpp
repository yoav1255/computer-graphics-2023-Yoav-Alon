#include "Camera.h"

Camera::Camera()
{
	view_transformation = glm::mat4x4();
	projection_transformation = glm::mat4x4();
}

Camera::~Camera()
{
	
}

void Camera::SetCameraLookAt(const glm::vec3& eye, const glm::vec3& at, const glm::vec3& up)
{
	view_transformation=glm::lookAt(eye, at, up);
}


const glm::mat4x4& Camera::GetProjectionTransformation() const
{
	return projection_transformation;
}

const glm::mat4x4& Camera::GetViewTransformation() const
{
	return view_transformation;
}

void Camera::SetTransformation(const glm::mat4x4& transform)
{
	view_transformation = transform;
}
void Camera::SetProjection(const glm::mat4x4& projection)
{
	projection_transformation = projection;
}
