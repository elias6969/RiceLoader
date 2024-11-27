#ifndef RICELOADER
#define RICELOADER

#include <vector>
#include <string>
#include <unordered_map>
#include <glm/glm.hpp>
#include "shader.h"
#include "camera.h"

// Vertex structure: holds position, texture coordinates, and normal data
struct Vertex {
    float x, y, z;         // Position
    float tx, ty;          // Texture Coordinates
    float nx, ny, nz;      // Normals
};

// Material structure: holds properties for lighting and texturing
struct Material {
    glm::vec3 ambient;      // Ambient color
    glm::vec3 diffuse;      // Diffuse color
    glm::vec3 specular;     // Specular color
    float shininess;        // Shininess coefficient
    std::string texturePath; // Path to the texture
    unsigned int textureID;  // OpenGL texture ID
    unsigned int diffuseTexture; // ID for the diffuse texture
    unsigned int specularTexture; // ID for the specular texture
};

// Mesh structure: encapsulates vertices, indices, and material data
struct Mesh {
    std::vector<Vertex> vertices;        // List of vertices
    std::vector<unsigned int> indices;   // List of indices for indexed rendering
    Material material;       
    unsigned int vao; // Vertex Array Object
    unsigned int vbo; // Vertex Buffer Object// Material properties
};

// Function declarations

// Load material data from a .mtl file
void LoadMaterial(const std::string& filePath, std::unordered_map<std::string, Material>& materials);

// Load model data from an .obj file into a vector of Mesh structs
void LoadModel(const std::string& filePath, std::vector<Mesh>& meshes);

// Upload mesh data to GPU buffers
void LoadMeshToGPU(const Mesh& mesh, unsigned int& vao, unsigned int& vbo, unsigned int& ebo);

// Draw a mesh using its associated VAO and shader
void Draw(
    unsigned int& vao, Shader& shader, Mesh& mesh, Material& material,
    Camera& camera, const unsigned int SCR_WIDTH, const unsigned int SCR_HEIGHT,
    glm::vec3 lightPos, glm::vec3 lightColor);

// Release GPU resources for a mesh
void Unload(unsigned int& vao, unsigned int& vbo, unsigned int& ebo);

#endif
