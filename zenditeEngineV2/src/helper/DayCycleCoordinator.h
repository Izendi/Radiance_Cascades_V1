#pragma once

#include "../ECS/Components.h"
#include "../utils.h"
#include "../ECS/ECSUtils.h"

#include "../Coordinator.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

class DayCycleCoordinator
{
public:
	Coordinator& COORD;

	Entity m_Sun;

	float m_SunRadius;

	float m_Angle = 0.0f;

	bool m_Halt = false;

	c_Transform* m_SunPosition;

	glm::vec3 m_SkyColor;
	glm::vec3 m_lightDirection;

	glm::vec3 m_dawn = glm::vec3(0.251f, 0.463f, 0.549f);
	glm::vec3 m_midday = glm::vec3(0.326f, 0.647f, 1.207f);
	glm::vec3 m_evening = glm::vec3(0.326f, 0.647f, 1.207f);
	glm::vec3 m_sunset = glm::vec3(0.7f, 0.3f, 0.2f) * 1.2f;
	glm::vec3 m_night = glm::vec3(0.0f, 0.0f, 0.1f);

	float m_SingleCycleDuration;
	float m_AngularSpeed;

	void SetUpCycle();

public:
	DayCycleCoordinator(Coordinator& coordinator, Entity Sun, float sunRadius, float singleCycleDuration);

	void Update(float deltaTime, float currentTime);

	void setSingleCycleDuration(float time);

	glm::vec3 getSkyColor() const;

	glm::vec3 getSunPosition() const;

	void Pause();

	void Resume();

};