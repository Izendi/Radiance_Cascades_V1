#include "Model.h"
#include "Node.h"
#include "../Shader.h"


Model::Model(const char* modelFilePath, Shader shader)
{
	
	filePath = modelFilePath;
	shaderHandle = &shader; //shallow copy
	const aiScene* assimpSceneObj = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

	rootNode = new Node(this);

	rootNode->ProcessNodeHierarchy(assimpSceneObj->mRootNode, assimpSceneObj);
}

void Model::Draw(Shader shader)
{
	shaderHandle = &shader; //Shallow copy (should not cause issues at the moment but could be problematic in the future)
	rootNode->RenderNodeHierarchy(shaderHandle);
}

const std::vector<Texture>& Model::getLoadedTexRef() const
{
	return loaded_textures;
}

std::string Model::getObjFilePath() const
{
	return filePath;
}

void Model::addLoadedTexture(Texture newTexture)
{
	loaded_textures.push_back(newTexture);
}




