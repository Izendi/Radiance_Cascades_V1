#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct c_Transform
{
	glm::vec3 pos;
	glm::vec3 scale;
};

struct c_RenderableComponent
{
	void setPosVertexArray(float* verts, size_t size)
	{
		posVertices = new float[size];

		for (int i = 0; i < size; ++i)
		{
			posVertices[i] = verts[i];
		}

		posArraySize = size;
	}

	void setSurfaceNormalVertexArray(float* verts, size_t size)
	{
		surfaceNormalVertices = new float[size];

		for (int i = 0; i < size; ++i)
		{
			surfaceNormalVertices[i] = verts[i];
		}

		snArraySize = size;
	}

	float* posVertices;
	float* surfaceNormalVertices; //#HERE
	size_t posArraySize;
	size_t snArraySize;
};

struct c_Texture
{
	void setTexCoordsVertexArray(float* verts, size_t size)
	{
		texCoords = new float[size];

		for (int i = 0; i < size; ++i)
		{
			texCoords[i] = verts[i];
		}

		arraySize = size;
	}

	unsigned short int texUnit;
	float* texCoords;
	size_t arraySize;
};

struct c_RigidBodyCollidable
{

};

struct c_ImmovableCollidable
{

};

struct c_CollidableTrigger
{

};

struct c_Modified
{
	bool isModifed;
};