#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb_image/stb_image.h>

#include "shader.h"
#include <iostream>
#include <fstream>
#include <string>
#include "riceLoader.h"
#include <vector>


void VertFacecounter(const std::string& filePath, std::vector<Vertex>& vertices, unsigned int& vao, unsigned int& vbo) {
    std::ifstream objFile(filePath);
    if (!objFile.is_open()) {
        std::cerr << "Error: Could not open file " << filePath << std::endl;
        return;
    }

    std::vector<glm::vec3> positions;  // To store vertex positions (v)
    std::vector<glm::vec2> texCoords; // To store texture coordinates (vt)
    std::vector<glm::vec3> normals;   // To store vertex normals (vn)

    std::string line;
    while (std::getline(objFile, line)) {
        // Parse vertex positions
        if (line.rfind("v ", 0) == 0) {
            float x, y, z;
            if (sscanf(line.c_str(), "v %f %f %f", &x, &y, &z) != 3) {
                std::cerr << "Error parsing vertex position: " << line << std::endl;
                continue;
            }
            positions.push_back(glm::vec3(x, y, z));
        }
        // Parse texture coordinates
        else if (line.rfind("vt ", 0) == 0) {
            float tx, ty;
            if (sscanf(line.c_str(), "vt %f %f", &tx, &ty) != 2) {
                std::cerr << "Error parsing texture coordinates: " << line << std::endl;
                continue;
            }
            texCoords.push_back(glm::vec2(tx, ty));
        }
        // Parse vertex normals
        else if (line.rfind("vn ", 0) == 0) {
            float nx, ny, nz;
            if (sscanf(line.c_str(), "vn %f %f %f", &nx, &ny, &nz) != 3) {
                std::cerr << "Error parsing vertex normal: " << line << std::endl;
                continue;
            }
            normals.push_back(glm::vec3(nx, ny, nz));
        }
        // Parse faces
        else if (line.rfind("f ", 0) == 0) {
            unsigned int vIdx[3], tIdx[3], nIdx[3];
            int matches = sscanf(
                line.c_str(),
                "f %u/%u/%u %u/%u/%u %u/%u/%u",
                &vIdx[0], &tIdx[0], &nIdx[0],
                &vIdx[1], &tIdx[1], &nIdx[1],
                &vIdx[2], &tIdx[2], &nIdx[2]
            );

            if (matches != 9) {
                std::cerr << "Error parsing face data: " << line << std::endl;
                continue;
            }

            // Convert indices (OBJ uses 1-based indexing)
            for (int i = 0; i < 3; i++) {
                Vertex vertex;
                vertex.x = positions[vIdx[i] - 1].x;
                vertex.y = positions[vIdx[i] - 1].y;
                vertex.z = positions[vIdx[i] - 1].z;

                vertex.tx = texCoords[tIdx[i] - 1].x;
                vertex.ty = texCoords[tIdx[i] - 1].y;

                vertex.nx = normals[nIdx[i] - 1].x;
                vertex.ny = normals[nIdx[i] - 1].y;
                vertex.nz = normals[nIdx[i] - 1].z;

                vertices.push_back(vertex);
            }
        }
    }

    objFile.close();

    std::cout << "Vertices: " << positions.size() << std::endl;
    std::cout << "Texture coordinates: " << texCoords.size() << std::endl;
    std::cout << "Normals: " << normals.size() << std::endl;
    std::cout << "Final Vertices (after faces processed): " << vertices.size() << std::endl;

    for (const auto& vertex : vertices) {
        std::cout << "Vertex: " << vertex.x << ", " << vertex.y << ", " << vertex.z
            << " | Texture Coords: " << vertex.tx << ", " << vertex.ty
            << " | Normals: " << vertex.nx << ", " << vertex.ny << ", " << vertex.nz << std::endl;
    }

    LoadVerts(vertices, vao, vbo);
}

void LoadVerts(std::vector<Vertex> &verts, unsigned int &vao, unsigned int &vbo) {
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(Vertex), verts.data(), GL_STATIC_DRAW);
	// Define how to interpret the vertex data (just position here)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(0);

	// Texture Coordinates attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, tx)));
	glEnableVertexAttribArray(1);

	// Normal attribute
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, nx)));
	glEnableVertexAttribArray(2);

}

void Draw(unsigned int& vao, Shader& shader, std::vector<Vertex>& verts){
    // Use the shader program
    shader.use();

    // View matrix for camera movement (simple orbiting camera)
    glm::mat4 view = glm::mat4(1.0f);
    float radius = 10.0f;
    float camX = static_cast<float>(sin(glfwGetTime()) * radius);
    float camZ = static_cast<float>(cos(glfwGetTime()) * radius);
    view = glm::lookAt(glm::vec3(camX, 0.0f, camZ), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    shader.setMat4("view", view);

    // Projection matrix (Perspective Projection)
    glm::mat4 projection = glm::perspective(
        glm::radians(45.0f),  // Field of view
        800.0f / 600.0f,      // Aspect ratio (adjust as per your window size)
        0.1f,                 // Near clipping plane
        100.0f                // Far clipping plane
    );
    shader.setMat4("projection", projection);

    // Model matrix (for positioning, scaling, and rotating the object)
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, static_cast<float>(glfwGetTime()), glm::vec3(0.0f, 1.0f, 5.0f));  // Continuous rotation
    shader.setMat4("model", model);

    // Bind the VAO and draw the object
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(verts.size()));
    glBindVertexArray(0);  // Unbind the VAO (good practice)
}

void Unload(unsigned int& vao, unsigned int& vbo) {
    // Delete the VAO and VBO
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);

    vao = 0;  // Reset to 0 for safety
    vbo = 0;
}
