#pragma once
#include <glm/glm.hpp>
#include <string>
#include "Face.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <glm/gtc/random.hpp>

class Light
{
public:
	Light(glm::vec3 pos = glm::vec3(1, 1, 1), glm::vec3 amb = glm::vec3(1, 1, 1), glm::vec3 dif = glm::vec3(0, 0, 0), glm::vec3 spec = glm::vec3(0,0,0));
	virtual ~Light();
	glm::mat4 get_transform();
	void set_transform(glm::vec3 trans);

	glm::vec3 position;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	glm::vec3 translation;
	glm::mat4 transform;
	bool flat_shading;
	bool phong_shading;
	bool gouraud_shading;
	float ambient_strength;
	float diffuse_strength;
	float specular_strength;
};
