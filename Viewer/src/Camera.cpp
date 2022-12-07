#include "Camera.h"

Camera::Camera():

	view_transformation(glm::mat4x4()),
	projection_transformation(glm::mat4x4()),
	objectTransform(glm::mat4(1.0)),
	worldTransform(glm::mat4(1.0)),
	translationObject(glm::vec3(1.0f)),
	scaleObject(glm::vec3(1, 1, 1)),
	rotationObject(glm::vec3(0, 0, 0)),
	translationWorld(glm::vec3(1.0f)),
	scaleWorld(glm::vec3(1, 1, 1)),
	rotationWorld(glm::vec3(0, 0, 0))
{
}

Camera::~Camera()
{
	
}

void Camera::SetCameraLookAt(const glm::vec3& eye, const glm::vec3& at, const glm::vec3& up)
{
	view_transformation=GetTransform()* glm::lookAt(eye, at, up);
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


glm::mat4x4 Camera::GetTransform()
{
	return worldTransform * objectTransform;
}

glm::mat4x4 Camera::GetObjectTransform()
{
	return objectTransform;
}

glm::mat4x4 Camera::GetWorldTransform()
{
	return worldTransform;
}

void Camera::SetObjectTransform()
{
	glm::mat4 rotateX = glm::rotate(glm::mat4(1.0f), glm::radians(rotationObject.x), glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 rotateY = glm::rotate(glm::mat4(1.0f), glm::radians(rotationObject.y), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 rotateZ = glm::rotate(glm::mat4(1.0f), glm::radians(rotationObject.z), glm::vec3(0.0f, 0.0f, 1.0f));

	glm::mat4 matRotation = rotateX * rotateY * rotateZ;
	glm::mat4 matTranslation = glm::translate(glm::mat4(1.0f), translationObject);
	glm::mat4 matScale = glm::scale(glm::mat4(1.0f), scaleObject);

	objectTransform = glm::inverse(matTranslation * matRotation * matScale);
}

void Camera::SetWorldTransform()
{
	glm::mat4 rotateX = glm::rotate(glm::mat4(1.0f), glm::radians(rotationWorld.x), glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 rotateY = glm::rotate(glm::mat4(1.0f), glm::radians(rotationWorld.y), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 rotateZ = glm::rotate(glm::mat4(1.0f), glm::radians(rotationWorld.z), glm::vec3(0.0f, 0.0f, 1.0f));

	glm::mat4 matRotation = rotateX * rotateY * rotateZ;
	glm::mat4 matTranslation = glm::translate(glm::mat4(1.0f), translationWorld);
	glm::mat4 matScale = glm::scale(glm::mat4(1.0f), scaleWorld);

	worldTransform = glm::inverse(matTranslation * matRotation * matScale);
}
