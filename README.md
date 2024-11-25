# RiceLoader 🍚
RiceLoader is a lightweight, self-configured model loader designed to simplify rendering 3D .obj models. This project is still in its early stages but already supports loading basic vertices, texture coordinates, and normals from .obj files and renders them using OpenGL.

Features
Loads .obj files: Reads vertex positions, texture coordinates, and normals.
Renders models: Displays 3D models with basic transformations and a simple camera view.
Self-contained: Lightweight and easy to integrate into your projects.
Work in Progress (WIP)
Material support: Integrating .mtl for advanced material rendering.
Enhanced face processing: Optimizing face data handling for complex models.
Texture mapping: Currently in development.
How It Works
Core Functions
1. Reading .obj Files
The VertFacecounter function processes .obj files, extracting:

Vertices (v),
Texture Coordinates (vt),
Normals (vn), and
Faces (f).
cpp
Copy code
void VertFacecounter(const std::string& filePath, std::vector<Vertex>& vertices, unsigned int& vao, unsigned int& vbo) {
    std::ifstream objFile(filePath);
    if (!objFile.is_open()) {
        std::cerr << "Error: Could not open file " << filePath << std::endl;
        return;
    }
    std::vector<glm::vec3> positions;  // Vertex positions
    std::vector<glm::vec2> texCoords; // Texture coordinates
    std::vector<glm::vec3> normals;   // Normals
    std::string line;
    while (std::getline(objFile, line)) {
        if (line.rfind("v ", 0) == 0) { // Parse vertices
            ...
        } else if (line.rfind("f ", 0) == 0) { // Parse faces
            ...
        }
    }
    objFile.close();
    LoadVerts(vertices, vao, vbo); // Pass data for rendering
}

2. Loading Vertices for Rendering
The LoadVerts function prepares vertex data for rendering by setting up VAOs and VBOs:

cpp
Copy code
void LoadVerts(std::vector<Vertex>& verts, unsigned int& vao, unsigned int& vbo) {
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(Vertex), verts.data(), GL_STATIC_DRAW);
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    // Texture coordinates
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, tx)));
    glEnableVertexAttribArray(1);
    // Normals
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, nx)));
    glEnableVertexAttribArray(2);
}

3. Rendering the Model
The Draw function uses a shader program to apply transformations and render the model. A simple camera and continuous rotation are implemented:

cpp
Copy code
void Draw(unsigned int& vao, Shader& shader, std::vector<Vertex>& verts) {
    shader.use();
    glm::mat4 view = glm::lookAt(glm::vec3(camX, 0.0f, camZ), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    shader.setMat4("view", view);
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
    shader.setMat4("projection", projection);
    glm::mat4 model = glm::rotate(glm::mat4(1.0f), static_cast<float>(glfwGetTime()), glm::vec3(0.0f, 1.0f, 5.0f));
    shader.setMat4("model", model);
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(verts.size()));
}

4. Cleaning Up
The Unload function deletes buffers to free memory:

cpp
Copy code
void Unload(unsigned int& vao, unsigned int& vbo) {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
}
Getting Started
Prerequisites
OpenGL 3.3+
GLFW: For window and input handling.
GLM: For math operations.
stb_image: For texture loading.
Build Instructions
Clone the repository:
bash
Copy code
git clone https://github.com/yourusername/RiceLoader.git
cd RiceLoader
Compile using your favorite build system (e.g., CMake, Makefile).
Example Output
Once loaded, RiceLoader displays .obj models with basic transformations like rotation:

Feel free to contribute by submitting issues or pull requests. Let's build something awesome! 🚀
