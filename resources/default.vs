#version 330 core

layout(location = 0) in vec3 aPos;       // Vertex position
layout(location = 1) in vec2 aTexCoord;  // Texture coordinate
layout(location = 2) in vec3 aNormal;    // Normal vector

out vec3 FragPos;       // To pass world position to fragment shader
out vec3 Normal;        // To pass normals to fragment shader
out vec2 TexCoord;      // To pass texture coordinates

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal; // Transform normals correctly
    TexCoord = aTexCoord;

    gl_Position = projection * view * vec4(FragPos, 1.0);
}

