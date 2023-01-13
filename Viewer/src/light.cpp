#include "light.h"

light::light(glm::vec3 pos, glm::vec3 amb, glm::vec3 dif, glm::vec3 spec):
position(pos),
ambient(amb),
diffuse(dif),
specular(spec)
{
	translation = glm::vec3(1.0f);
	transform = glm::mat4(1.0f);
}
light::~light()
{

}
glm::mat4 light::get_transform()
{
	return transform;
}
void light::set_transform(glm::vec3 trans)
{
	transform = glm::translate(glm::mat4(1.0f), trans);;
}
