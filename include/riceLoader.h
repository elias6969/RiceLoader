#ifndef RICELOADER
#define RICELAODER
#include <vector>
#include <iostream>
#include "shader.h"

struct Vertex
{
	float x, y, z;
	float tx, ty;  // Texture Coordinates
	float nx, ny, nz; // Normals
};

void VertFacecounter(const std::string& filePath, std::vector<Vertex> &vertices, unsigned int &vao, unsigned int &vbo);

void LoadVerts(std::vector<Vertex>& verts, unsigned int& vao, unsigned int& vbo);

void Draw(unsigned int& vao, Shader& shader, std::vector<Vertex> &verts);

void Unload(unsigned int& vao, unsigned int& vbo);

#endif 