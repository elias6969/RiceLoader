#version 330 core

struct Material {
    sampler2D diffuse;  // Diffuse texture
    sampler2D specular; // Specular texture
    float shininess;    // Shininess factor
};

in vec3 FragPos;      // World position of the fragment
in vec3 Normal;       // Interpolated normal vector
in vec2 TexCoord;     // Interpolated texture coordinates

out vec4 FragColor;

uniform vec3 lightPos;     // Light source position
uniform vec3 viewPos;      // Camera/view position
uniform vec3 lightColor;   // Light color
uniform Material material; // Material properties

void main() {
    // Ambient lighting
    vec3 ambient = 0.1 * lightColor;

    // Diffuse lighting
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor * texture(material.diffuse, TexCoord).rgb;

    // Specular lighting
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = spec * lightColor * texture(material.specular, TexCoord).rgb;

    // Combine lighting components
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}
