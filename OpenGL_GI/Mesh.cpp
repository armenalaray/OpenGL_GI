#include "Mesh.h"

Mesh::Mesh(vector<Vertex> vertices, vector<Texture> textures, vector<unsigned int> indices)
{
	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;
	createGLObjects();
}

Mesh::Mesh(const Mesh& other)
{
	this->vAO = other.vAO;
	this->vBO = other.vBO;
	this->eBO = other.eBO;
	this->vertices = other.vertices;
	this->indices = other.indices;
	this->textures = other.textures;
}

Mesh Mesh::operator=(const Mesh& other)
{
	return Mesh(other);
}

Mesh::~Mesh()
{
	//glDeleteVertexArrays(1, &vAO);
	//glCheckError();
	//glDeleteBuffers(1, &vBO);
	//glCheckError();
	//glDeleteBuffers(1, &eBO);
	//glCheckError();
}

void Mesh::Draw(Shader& shader)
{
	unsigned int diffIndex = 0;
	unsigned int specIndex = 0;

	glBindVertexArray(vAO);
	glCheckError();

	//NOTE: text unit 0 is for cubemap!!!
	unsigned int texUnit = 1;
	for (unsigned int texIndex = 0; texIndex < textures.size(); ++texIndex)
	{
		string unitString = "";
		switch (textures[texIndex].type)
		{
		case TEX_TYPE_DIFFUSE:
		{
			unitString = "mat.texDiff_" + to_string(diffIndex++);
		}break;
		case TEX_TYPE_SPECULAR:
		{
			unitString = "mat.texSpec_" + to_string(specIndex++);
		}break;

		}
		
		shader.setVar(unitString.c_str(), (int)texUnit);
		glActiveTexture(GL_TEXTURE0 + texUnit);
		glCheckError();
		glBindTexture(GL_TEXTURE_2D, textures[texIndex].id);
		glCheckError();
	}

	glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);
	glCheckError();
	glBindVertexArray(0);
	glCheckError();
	glBindTexture(GL_TEXTURE_2D, 0);
	glCheckError();

	//NOTE: set default state after drawing
	glActiveTexture(GL_TEXTURE0);
	glCheckError();
}

void Mesh::createGLObjects()
{
	glGenVertexArrays(1, &vAO);
	glCheckError();
	glBindVertexArray(vAO);
	glCheckError();

	glGenBuffers(1, &vBO);
	glCheckError();
	glBindBuffer(GL_ARRAY_BUFFER, vBO);
	glCheckError();
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
	glCheckError();

	glGenBuffers(1, &eBO);
	glCheckError();
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eBO);
	glCheckError();
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
	glCheckError();

	//vertex position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glCheckError();
	glEnableVertexAttribArray(0);
	glCheckError();

	//vertex normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, n));
	glCheckError();
	glEnableVertexAttribArray(1);
	glCheckError();

	//texture coordinates attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tC));
	glCheckError();
	glEnableVertexAttribArray(2);
	glCheckError();

	glBindVertexArray(0);
	glCheckError();
}
