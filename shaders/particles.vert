#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 speed;
out VS_OUT {
    vec3 speed;
} vs_out;

uniform mat4 model;
layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

void main() {
    gl_PointSize = 2;
    gl_Position = projection * view * model * vec4(position, 1.0f);
    vs_out.speed = speed;
}