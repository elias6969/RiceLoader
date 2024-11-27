#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb_image/stb_image.h>
#include <unordered_map>
#include "shader.h"
#include "camera.h"
#include "riceLoader.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>




// Parse material data from an MTL file
void LoadMaterial(const std::string& filePath, std::unordered_map<std::string, Material>& materials) {
    std::ifstream mtlFile(filePath);
    if (!mtlFile.is_open()) {
        std::cerr << "Error: Could not open MTL file " << filePath << std::endl;
        return;
    }

    Material currentMaterial;
    std::string line, currentMaterialName;

    while (std::getline(mtlFile, line)) {
        std::istringstream lineStream(line);
        std::string token;
        lineStream >> token;

        if (token == "newmtl") {
            if (!currentMaterialName.empty()) {
                materials[currentMaterialName] = currentMaterial;
            }
            lineStream >> currentMaterialName;
            currentMaterial = Material();
        }
        else if (token == "Ka") {
            lineStream >> currentMaterial.ambient.r >> currentMaterial.ambient.g >> currentMaterial.ambient.b;
        }
        else if (token == "Kd") {
            lineStream >> currentMaterial.diffuse.r >> currentMaterial.diffuse.g >> currentMaterial.diffuse.b;
        }
        else if (token == "Ks") {
            lineStream >> currentMaterial.specular.r >> currentMaterial.specular.g >> currentMaterial.specular.b;
        }
        else if (token == "Ns") {
            lineStream >> currentMaterial.shininess;
        }
        else if (token == "map_Kd") {
            lineStream >> currentMaterial.texturePath;
        }
    }

    if (!currentMaterialName.empty()) {
        materials[currentMaterialName] = currentMaterial;
    }
}

// Parse OBJ file and load meshes
void LoadModel(const std::string& filePath, std::vector<Mesh>& meshes) {
    std::ifstream objFile(filePath);
    if (!objFile.is_open()) {
        std::cerr << "Error: Could not open OBJ file " << filePath << std::endl;
        return;
    }

    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> texCoords;
    std::vector<glm::vec3> normals;
    std::unordered_map<std::string, Material> materials;

    Mesh currentMesh;
    std::string line, currentMaterialName;

    while (std::getline(objFile, line)) {
        std::istringstream lineStream(line);
        std::string token;
        lineStream >> token;

        if (token == "v") {
            glm::vec3 position;
            lineStream >> position.x >> position.y >> position.z;
            positions.push_back(position);
        }
        else if (token == "vt") {
            glm::vec2 texCoord;
            lineStream >> texCoord.x >> texCoord.y;
            texCoords.push_back(texCoord);
        }
        else if (token == "vn") {
            glm::vec3 normal;
            lineStream >> normal.x >> normal.y >> normal.z;
            normals.push_back(normal);
        }
        else if (token == "f") {
            unsigned int posIdx, texIdx, normIdx;
            std::string faceData;

            while (lineStream >> faceData) {
                if (std::sscanf(faceData.c_str(), "%u/%u/%u", &posIdx, &texIdx, &normIdx) == 3) {
                    Vertex vertex;
                    vertex.x = positions[posIdx - 1].x;
                    vertex.y = positions[posIdx - 1].y;
                    vertex.z = positions[posIdx - 1].z;

                    vertex.tx = texCoords[texIdx - 1].x;
                    vertex.ty = texCoords[texIdx - 1].y;

                    vertex.nx = normals[normIdx - 1].x;
                    vertex.ny = normals[normIdx - 1].y;
                    vertex.nz = normals[normIdx - 1].z;

                    currentMesh.vertices.push_back(vertex);
                    currentMesh.indices.push_back(static_cast<unsigned int>(currentMesh.vertices.size() - 1));
                }
            }
        }
        else if (token == "usemtl") {
            if (!currentMesh.vertices.empty()) {
                meshes.push_back(currentMesh);
                currentMesh = Mesh();
            }
            lineStream >> currentMaterialName;
            currentMesh.material = materials[currentMaterialName];
        }
        else if (token == "mtllib") {
            std::string mtlFile;
            lineStream >> mtlFile;
            LoadMaterial(mtlFile, materials);
        }
    }

    if (!currentMesh.vertices.empty()) {
        meshes.push_back(currentMesh);
    }
}

// Bind mesh data to GPU
void LoadMeshToGPU(const Mesh& mesh, unsigned int& vao, unsigned int& vbo, unsigned int& ebo) {
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(Vertex), mesh.vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(unsigned int), mesh.indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, tx)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, nx)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

// Render the mesh
void Draw(
    unsigned int& vao, Shader& shader, Mesh& mesh, Material& material,
    Camera& camera, const unsigned int SCR_WIDTH, const unsigned int SCR_HEIGHT,
    glm::vec3 lightPos, glm::vec3 lightColor)
{
    // Use the shader program
    shader.use();

    // Set view matrix from camera
    glm::mat4 view = camera.GetViewMatrix();
    shader.setMat4("view", view);

    // Set projection matrix (Perspective Projection)
    glm::mat4 projection = glm::perspective(
        glm::radians(camera.Zoom), // Field of view
        static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT), // Aspect ratio
        0.1f, // Near clipping plane
        100.0f // Far clipping plane
    );
    shader.setMat4("projection", projection);

    // Set model matrix (Positioning, Scaling, Rotation)
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, static_cast<float>(glfwGetTime()), glm::vec3(0.0f, 1.0f, 0.0f)); // Continuous rotation
    shader.setMat4("model", model);

    // Set light properties
    shader.setVec3("lightPos", lightPos);
    shader.setVec3("lightColor", lightColor);

    // Set camera position
    shader.setVec3("viewPos", camera.Position);

    // Bind material properties
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, material.diffuseTexture);
    shader.setInt("material.diffuse", 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, material.specularTexture);
    shader.setInt("material.specular", 1);

    shader.setFloat("material.shininess", material.shininess);

    // Bind the VAO and draw the object
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(mesh.vertices.size()));
    glBindVertexArray(0); // Unbind the VAO (good practice)
}


// Free resources
void Unload(unsigned int& vao, unsigned int& vbo, unsigned int& ebo) {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);

    vao = 0;
    vbo = 0;
    ebo = 0;
}

