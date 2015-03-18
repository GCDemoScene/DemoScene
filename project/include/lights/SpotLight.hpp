#pragma once

struct SpotLight
{
	glm::vec3 position;
	float angle;
	glm::vec3 direction;
	float penumbraAngle;
	glm::vec3 color;
	float intensity;
	glm::mat4 worldToLightScreen;	
};