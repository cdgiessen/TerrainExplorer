#pragma once

#include <iostream>
// GL Includes
#include <glm/glm.hpp>

class Bullet
{
public:
	glm::vec3 Position;
	glm::vec3 Motion;
	// Eular Angles
	GLfloat Yaw;
	GLfloat Pitch;

	bool ToBeDestroyed = false;
	const float MASS = 2.0f;
	const float GRAVITY = -9.8f;

	Bullet(glm::vec3 position, glm::vec3 motion, GLfloat yaw, GLfloat pitch) {
		Position = position;
		Motion = motion;
		Yaw = yaw;
		Pitch = pitch;
	}

	void updatePosition(float terrainHeight, float deltaTime) {
		if (terrainHeight < Position.y) {
			Motion.y += MASS * GRAVITY * deltaTime;
			Position += Motion * deltaTime; 
			//std::cout << Position.y << std::endl;
		}
		else {
			ToBeDestroyed = true;
			 std::cout << "Bullet Destroyed" << std::endl;
		}
	}

};

