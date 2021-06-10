#include "Model.h"

Model::Model(const string & path)
{
	stbi_set_flip_vertically_on_load(true);
	LoadModel(path);
}

void Model::Draw(Shader& shader)
{
	for (Mesh mesh : meshes)
	{
		mesh.Draw(shader);
	}
}

void Model::LoadModel(const string& path)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
	}
	else
	{
		directory = path.substr(0, path.find_last_of('\\'));
		ProcessNode(scene->mRootNode, scene);
	}
}

void Model::ProcessNode(aiNode* node, const aiScene* scene)
{
	for (unsigned int i = 0; i < node->mNumMeshes; ++i)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(ProcessMesh(mesh, scene));
	}

	for (unsigned int i = 0; i < node->mNumChildren; ++i)
	{
		ProcessNode(node->mChildren[i], scene);
	}
}

Mesh Model::ProcessMesh(aiMesh* mesh, const aiScene* scene)
{
	vector<Vertex> vertices;
	vector<Texture> textures;
	vector<unsigned int> indices;

	for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
	{
		Vertex v = {};
		v.p.x = mesh->mVertices[i].x;
		v.p.y = mesh->mVertices[i].y;
		v.p.z = mesh->mVertices[i].z;

		if (mesh->HasNormals())
		{
			v.n.x = mesh->mNormals[i].x;
			v.n.y = mesh->mNormals[i].y;
			v.n.z = mesh->mNormals[i].z;
		}

		if (mesh->mTextureCoords[0])
		{
			v.tC.x = mesh->mTextureCoords[0][i].x;
			v.tC.y = mesh->mTextureCoords[0][i].y;
		}
		else
		{
			//NOTE: we are adding fake texture coordinates!!!
			v.tC = glm::vec2(0, 0);
		}

		vertices.push_back(v);
	}

	for (unsigned int faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex)
	{
		aiFace face = mesh->mFaces[faceIndex];
		for (unsigned int i = 0; i < face.mNumIndices; ++i)
		{
			indices.push_back(face.mIndices[i]);
		}
	}

	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];
		vector<Texture> vDiffTex = LoadMaterialTextures(mat, aiTextureType_DIFFUSE, TEX_TYPE_DIFFUSE);
		textures.insert(textures.end(), vDiffTex.begin(), vDiffTex.end());
		vector<Texture> vSpecTex = LoadMaterialTextures(mat, aiTextureType_SPECULAR, TEX_TYPE_SPECULAR);
		textures.insert(textures.end(), vSpecTex.begin(), vSpecTex.end());
	}

	return Mesh(vertices, textures, indices);
}

vector<Texture> Model::LoadMaterialTextures(aiMaterial* mat, aiTextureType aiTexType, Tex_Type texType)
{
	vector<Texture> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(aiTexType); ++i)
	{
		aiString path;
		mat->GetTexture(aiTexType, i, &path);
		Texture texture;
		if (TextureLoaded(path.C_Str(), texture))
		{
			textures.push_back(texture);
		}
		else
		{
			texture.id = LoadTexture(path.C_Str(), directory);
			texture.type = texType;
			texture.fullPath = path.C_Str();
			textures.push_back(texture);
			texTable.push_back(texture);
		}
	}
	return textures;
}

unsigned int Model::LoadTexture(const char* texFileName_, string path)
{
	string texFileName = string(texFileName_);
	string fileFullPath = path + "\\" + texFileName;
	
	int width, height, chaCount;
	unsigned char* data = stbi_load(fileFullPath.c_str(), &width, &height, &chaCount, 0);

	if (data)
	{
		unsigned int id;
		glGenTextures(1, &id);
		glCheckError();
		glBindTexture(GL_TEXTURE_2D, id);
		glCheckError();

		//set the texture wrapping/filtering options (on the currently bound texture object)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glCheckError();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glCheckError();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glCheckError();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glCheckError();

		GLenum dataFormat;
		switch (chaCount)
		{
		case 1:
		{
			dataFormat = GL_RED;
		}break;
		case 3:
		{
			dataFormat = GL_RGB;
		}break;
		case 4:
		{
			dataFormat = GL_RGBA;
		}break;
		}

		glTexImage2D(GL_TEXTURE_2D, 0, dataFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
		glCheckError();
		glGenerateMipmap(GL_TEXTURE_2D);
		glCheckError();

		std::cout << "STB_IMAGE::LOADING_TEXTURE_SUCCESS" << std::endl;
		glBindTexture(GL_TEXTURE_2D, 0);
		glCheckError();
		stbi_image_free(data);

		return id;
	}
	else
	{
		std::cout << "ERROR::STB_IMAGE::FAILED TO LOAD TEXTURE" << std::endl;
		return -1;
	}
}

bool Model::TextureLoaded(const char* path, Texture& texture)
{
	for (int i = 0; i < texTable.size(); ++i)
	{
		if (strcmp(path, texTable[i].fullPath.c_str()) == 0)
		{
			texture = texTable[i];
			return true;
		}
	}
	return false;
}
