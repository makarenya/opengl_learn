#version 330 core

layout (location = 0) in vec3 vertex;

uniform mat4 view;
uniform mat4 projection;

out vec3 fragmentCoord;

void main() {
    gl_Position = (projection * view * vec4(vertex, 1.0f)).xyww;
    fragmentCoord = vec3(vec4(vertex, 1.0f));
}