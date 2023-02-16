#include "Light.h"

Light::Light(glm::vec3 pos, glm::vec3 amb, glm::vec3 dif, glm::vec3 spec):
position(pos),
ambient(amb),
diffuse(dif),
specular(spec)
{
	translation = glm::vec3(0.3f,0.1f,1.0f);
	transform = glm::mat4(1.0f);
	flat_shading = false;
	gouraud_shading = false;
	phong_shading = true;
}
Light::~Light()
{

}
glm::mat4 Light::get_transform()
{
	return transform;
}
void Light::set_transform(glm::vec3 trans)
{
	transform = glm::translate(glm::mat4(1.0f), trans);;
}
