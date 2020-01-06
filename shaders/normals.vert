#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

uniform mat4 model;
layout(std140) uniform Matrices {
    mat4 projection;
    mat4 view;
};

out VS_OUT {
    vec3 normal;
} vs_out;

void main() {
    gl_Position = projection * view * model * vec4(position, 1.0);
    mat3 nm = mat3(transpose(inverse(view * model)));
    vs_out.normal = normalize(vec3(projection * vec4(nm * normal, 0.0)));
}