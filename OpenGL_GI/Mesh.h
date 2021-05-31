#pragma once

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <vector>
#include <string>
#include "Shader.h"
#include "Utility.h"

using namespace std;

enum Tex_Type : unsigned int
{
	TEX_TYPE_DIFFUSE, TEX_TYPE_SPECULAR
};

struct Vertex 
{
	Vertex()
	{
		p = glm::vec3(0);
		n = glm::vec3(0);
		tC = glm::vec3(0);
	}

	glm::vec3 p;
	glm::vec3 n;
	glm::vec2 tC;
};

struct Texture
{
	unsigned int id;
	Tex_Type type;
	string fullPath;
};
class Mesh
{
public:
	Mesh(vector<Vertex> vertices, vector<Texture> textures, vector<unsigned int> indices);
	Mesh(const Mesh& other);
	Mesh operator=(const Mesh& other);
	~Mesh();
	void Draw(Shader& shader);
private:
	void createGLObjects();
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	vector<Texture> textures;
	unsigned int vAO, vBO, eBO;
};

