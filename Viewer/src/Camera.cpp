#include "Camera.h"

Camera::Camera():

	view_transformation(glm::mat4x4()),
	projection_transformation(glm::mat4x4()),
	objectTransform(glm::mat4(1.0f)),
	worldTransform(glm::mat4(1.0f)),
	translationObject(glm::vec3(1.0f)),
	scaleObject(glm::vec3(1, 1, 1)),
	rotationObject(glm::vec3(0, 0, 0)),
	translationWorld(glm::vec3(1.0f)),
	scaleWorld(glm::vec3(1, 1, 1)),
	rotationWorld(glm::vec3(0, 0, 0)),
	dolly(0.0f),
	fov(45.0f),
	zNearPerspective(1.0f),
	zFarPerspective(100.0f)
{
}

Camera::~Camera()
{
	
}
void Camera::setDolly(float distance)
{
	view_transformation = glm::translate(view_transformation, glm::vec3(0.0f, 0.0f, distance));

}

void Camera::SetCameraLookAt(const glm::vec3& eye, const glm::vec3& at, const glm::vec3& up)
{
	view_transformation= glm::lookAt(eye, at, up)  * GetTransform();
}

void Camera::Perspective(float aspect)
{
	//fov = fov + (fov * dolly * 0.5);
	projection_transformation = glm::perspective(glm::radians(fov), aspect, zNearPerspective, zFarPerspective);
}
void Camera::Ortho(float left, float right, float bottom, float top, float znear, float zfar)
{
	projection_transformation = glm::ortho(left, right, bottom, top, znear, zfar);

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
{// (AB)^-1 = B^-1 * A^-1
	
	return   worldTransform*objectTransform  ; 
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

	//objectTransform = glm::inverse(matTranslation * matRotation * matScale);
	objectTransform = (matTranslation * matRotation * matScale);
}

void Camera::SetWorldTransform()
{
	glm::mat4 rotateX = glm::rotate(glm::mat4(1.0f), glm::radians(rotationWorld.x), glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 rotateY = glm::rotate(glm::mat4(1.0f), glm::radians(rotationWorld.y), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 rotateZ = glm::rotate(glm::mat4(1.0f), glm::radians(rotationWorld.z), glm::vec3(0.0f, 0.0f, 1.0f));

	glm::mat4 matRotation = rotateX * rotateY * rotateZ;
	glm::mat4 matTranslation = glm::translate(glm::mat4(1.0f), translationWorld);
	glm::mat4 matScale = glm::scale(glm::mat4(1.0f), scaleWorld);

	//worldTransform = glm::inverse(matTranslation * matRotation * matScale);
	worldTransform = (matTranslation * matRotation * matScale);


}
