#include <iostream>
#include "Skydome.h"

Skydome::Skydome()
{
	//renderable.notWater = false;
}

void Skydome::DestroySkydome()
{
	//remove component data such as vertices from the component structure:

	//rend
	renderable.clear();
	trans.clear();
	mod.isModifed = true;
}

void Skydome::CreateSkydome(unsigned nLats, unsigned nlongs, float fRadius, glm::vec3 worldPosition, glm::vec3 scale)
{
	//DEBUG_ASSERT(nResolution > 0 && fRadius > 0, "Resolution or Radius can't be below 0");
	//DEBUG_ASSERT(fVertSweep > 0 && fVertSweep <= 90, "vertical Sweep must be between 0 - 90 degrees");

	//nlats = 4;
	//nlongs = 8;

	//therefore num of vertices = 8 * 4 + 1 = 33

	DestroySkydome();

	info.name = "Skydome";
	mod.isModifed = true;
	texUnit.is3Dtex = true;

	m_worldOrigin = worldPosition;

	setSkydomeTransform(m_worldOrigin, scale);

	m_Radius = fRadius;
	

	unsigned m_nVertices = nLats * nlongs + 1;

	//renderable.vertices;

	//Assign the vertex data to the renderable component:

	//First vertex is at the north pole, it is a special case and as such is done outside the loop:
	Vertex vertex;

	glm::vec3 posData;
	posData.x = 0.0f;
	posData.y = fRadius;
	posData.z = 0.0f;

	glm::vec3 normData;
	normData.x = 0.0f;
	normData.y = -1.0f;
	normData.z = 0.0f;

	glm::vec2 texCoordData;
	texCoordData.x = 0.5f;
	texCoordData.y = 0.5f;

	vertex.Position = posData;
	vertex.Normal = normData;
	vertex.TexCoords = texCoordData;

	renderable.vertices.push_back(vertex);

	float latOffset = (float)m_Radius / (float)nLats;
	float longOffset = 360.0f / (float)nlongs;

	float angleOfRotation = glm::radians(longOffset);

	float verticalRotationOffset = 90.0f / (float)nLats;

	float z = latOffset;
	float x = 0.0f;
	float y = fRadius;

	float x_new = 0.0f;
	float z_new = latOffset;

	for (size_t i = 0; i < nLats; ++i)
	{
		z_new = (latOffset * (i + 1));
		x_new = 0.0f;
		float rotationVal = verticalRotationOffset * (float)i;

		y = fRadius * std::cos(glm::radians(rotationVal));
		
		//Do one rotation around the circle for each of these loops (nLongs tells us how many angle increments we have)
		for (size_t ii = 0; ii < nlongs; ++ii)
		{
			x = x_new;
			z = z_new;
			//y = i + i * 0.2;

			Vertex vert;

			glm::vec3 pos;
			pos.x = x_new;
			pos.y = y;
			pos.z = z_new;

			glm::vec3 norm;
			norm.x = 0.0f;
			norm.y = -1.0f;
			norm.z = 0.0f;

			glm::vec2 texCoord;
			texCoord.x = (x_new - (-m_Radius))/(m_Radius - (-m_Radius));
			texCoord.y = (z_new - (-m_Radius)) / (m_Radius - (-m_Radius));

			vert.Position = pos;
			vert.Normal = norm;
			vert.TexCoords = texCoord;

			renderable.vertices.push_back(vert);

			//rotate the vector to point to the next vertex:
			x_new = x * std::cos(angleOfRotation) - z * std::sin(angleOfRotation);
			z_new = x * std::sin(angleOfRotation) + z * std::cos(angleOfRotation);

		}
	}

	//Create the indices

	//assign the triangles for the triangle fan like structure around the north pole of the dome.
	for(int i = 1; i < nlongs; ++i)
	{
		renderable.indices.push_back(0);
		renderable.indices.push_back(i);
		renderable.indices.push_back(i + 1);
	}

	renderable.indices.push_back(0);
	renderable.indices.push_back(nlongs); //#Check_This: might be nlongs + 1 if there are problems
	renderable.indices.push_back(1);

	for(int i = 1; i < nLats; ++i)
	{
		for(int ii = 1; ii < nlongs; ++ii)
		{
			renderable.indices.push_back(ii + (i - 1) * nlongs);
			renderable.indices.push_back(ii + 1 + (i * nlongs));
			renderable.indices.push_back(ii + 1 + (i - 1) * nlongs);

			renderable.indices.push_back(ii + (i-1)*nlongs);
			renderable.indices.push_back(ii + (i * nlongs));
			renderable.indices.push_back(ii + 1 + (i * nlongs));

		}

		renderable.indices.push_back(nlongs + (i - 1) * nlongs);
		renderable.indices.push_back(1 + i * nlongs);
		renderable.indices.push_back(1 + (i - 1) * nlongs);

		renderable.indices.push_back(nlongs + (i - 1) * nlongs);
		renderable.indices.push_back(nlongs + i * nlongs);
		renderable.indices.push_back(1 + i * nlongs);
	}
	
	//std::cout << " --  ";


}

void Skydome::setSkydomeTransform(glm::vec3 worldPos, glm::vec3 scale)
{
	trans.modelMat.clear();
	trans.modelMat.shrink_to_fit();

	glm::mat4 modelMat = glm::mat4(1.0f);
	modelMat = glm::translate(modelMat, worldPos);
	modelMat = glm::scale(modelMat, scale);
	trans.modelMat.push_back(modelMat);
}

void Skydome::generate3DNoiseTexture(unsigned short texUnitToAssign, int x, int y, int z)
{
	texUnit.is3Dtex = true;
	GLCALL(glActiveTexture(GL_TEXTURE0 + texUnitToAssign));


}

void Skydome::setSkydomeTexUnit(unsigned short skydomeTexUnit)
{
	texUnit.texUnit = skydomeTexUnit;
}

void Skydome::SetComponent_c_Transform(c_Transform transform)
{

}

void Skydome::SetComponent_c_Renderable(c_Renderable renderableComponent)
{

}

void Skydome::SetComponent_c_Texture(c_Texture texture)
{

}

void Skydome::SetComponent_c_EntityInfo(c_EntityInfo information)
{

}

void Skydome::SetComponent_c_Modified(bool modified)
{

}

c_Transform& Skydome::GetComponent_Transform()
{
	return trans;
}

c_Renderable& Skydome::GetComponent_Renderable()
{
	return renderable;
}

c_Texture& Skydome::GetComponent_Texture()
{
	return texUnit;
}

c_EntityInfo& Skydome::GetComponent_EntityInfo()
{
	return info;
}

c_Modified& Skydome::GetComponent_Modified()
{
	return mod;
}

