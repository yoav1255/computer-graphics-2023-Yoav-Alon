#include "light.h"

light::light(glm::vec3 pos, glm::vec3 amb, glm::vec3 dif, glm::vec3 spec):
position(pos),
ambient(amb),
diffuse(dif),
specular(spec)
{
	
}
light::~light()
{

}