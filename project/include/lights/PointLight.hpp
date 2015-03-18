#pragma once

struct PointLight
{
	glm::vec3 position;
	int padding; // Padding is here to fit with OpenGL data aligment
	glm::vec3 color;
	float intensity;
	glm::mat4 worldToLightScreen;	
};