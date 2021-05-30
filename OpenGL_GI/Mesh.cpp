#include "Mesh.h"

Mesh::Mesh(vector<Vertex> vertices, vector<Texture> textures, vector<unsigned int> indices)
{
	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;
	createGLObjects();
}

Mesh::~Mesh()
{
	glDeleteVertexArrays(1, &vAO);
	glDeleteBuffers(1, &vBO);
	glDeleteBuffers(1, &eBO);
}

void Mesh::Draw(Shader& shader)
{
	unsigned int diffIndex = 0;
	unsigned int specIndex = 0;
	for (unsigned int texUnit = 0; texUnit < textures.size(); ++texUnit)
	{
		string unitString = "";
		switch (textures[texUnit].type)
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
		glBindTexture(GL_TEXTURE_2D, textures[texUnit].id);
	}

	glBindVertexArray(vAO);
	glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	//NOTE: set default state after drawing
	glActiveTexture(GL_TEXTURE0);
}

void Mesh::createGLObjects()
{
	glGenVertexArrays(1, &vAO);
	glBindVertexArray(vAO);
	
	glGenBuffers(1, &vBO);
	glBindBuffer(GL_ARRAY_BUFFER, vBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &eBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	//vertex position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(0);

	cout << offsetof(Vertex, n) << endl;
	cout << offsetof(Vertex, tC) << endl;

	////vertex color attribute
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(float)));
	//glEnableVertexAttribArray(1);

	//vertex normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, n));
	glEnableVertexAttribArray(1);

	//texture coordinates attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tC));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);
}
