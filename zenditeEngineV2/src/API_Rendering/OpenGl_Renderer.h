#pragma once
#include "I_Renderer.h"

class ECSCoordinator;

class OpenGL_Renderer : public I_Renderer
{
public:
	OpenGL_Renderer(std::shared_ptr<Camera> cam);
	void Render(const R_DataHandle& DataHandle, const c_Transform& trans) override;
	void RenderAABB(const R_DataHandle& DataHandle, 
		Shader& AABBShader,
		const c_AABB& AABB_Data, 
		const c_Transform& trans) override;
	void RenderLighting(const R_DataHandle& DataHandle,
		const c_Transform& trans,
		const ECSCoordinator& ECScoord,
		std::vector<Entity>& SpotLightEntities,
		std::vector<Entity>& PointLightEntities
	) override;

private:


};