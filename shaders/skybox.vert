#version 330 core

layout (location = 0) in vec3 vertex;
layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

out vec3 fragmentCoord;

void main() {
    gl_Position = (projection * mat4(mat3(view)) * vec4(vertex, 1.0f)).xyww;
    fragmentCoord = vec3(vec4(vertex, 1.0f));
}