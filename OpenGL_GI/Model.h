#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"
#include "stb_image.h"

class Model
{
public:
	Model(const string& path);
	void Draw(Shader& shader);
private:
	void LoadModel(const string& path);
	void ProcessNode(aiNode* node, const aiScene* scene);
	Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
	vector<Texture> LoadMaterialTextures(aiMaterial* mat, aiTextureType aiTexType, Tex_Type texType);
	unsigned int LoadTexture(const char* texFileName, string path);
	bool TextureLoaded(const char* path, Texture& texture);

	vector<Mesh> meshes;
	string directory;

	vector<Texture> texTable;
};

