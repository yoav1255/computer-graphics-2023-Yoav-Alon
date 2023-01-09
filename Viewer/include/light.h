#pragma once
#include <glm/glm.hpp>
#include <string>
#include "Face.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <glm/gtc/random.hpp>

class light
{
public:
	light(glm::vec3 pos = glm::vec3(1, 1, 1), glm::vec3 amb = glm::vec3(1, 1, 1), glm::vec3 dif = glm::vec3(0, 0, 0), glm::vec3 spec = glm::vec3(0,0,0));
	virtual ~light();

	glm::vec3 position;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	bool is_on;
	float ambient_strength;
};
