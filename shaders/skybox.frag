#version 330 core

struct Material {
    samplerCube skybox;
};

in vec3 fragmentCoord;
out vec4 color;
uniform Material material;

void main() {
    color = texture(material.skybox, fragmentCoord);
}