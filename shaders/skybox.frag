#version 330 core

in vec3 fragmentCoord;
out vec4 color;
uniform samplerCube skybox;

const vec4 gamma = vec4(1.0 / 2.2, 1.0 / 2.2, 1.0 / 2.2, 1.0);

void main() {
    color = pow(texture(skybox, fragmentCoord), gamma);
}