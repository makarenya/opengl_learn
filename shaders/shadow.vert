#version 330 core

layout (location = 0) in vec3 position;
layout (location = 2) in vec2 coord;

uniform mat4 lightMatrix;
uniform mat4 model;
out vec2 fragmentCoord;

void main() {
    gl_Position = lightMatrix * model * vec4(position, 1.0);
    fragmentCoord = coord;
}