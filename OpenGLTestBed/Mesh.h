#pragma once
#include <GLM\glm.hpp>
#include <string>
#include <vector>
#include "Shader.h"
class Mesh
{
public:
	struct Vertex
	{
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TexCoords;
	};
	struct Texture {
		unsigned int id;
		std::string type;
		std::string path;
	};

public:
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;
	unsigned int VAO;

	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
	void Draw(Shader shader);
	~Mesh();
private:
	unsigned int VBO, EBO;
	void setUpMesh();
};

