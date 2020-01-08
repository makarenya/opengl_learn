#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 coord;
layout (location = 3) in vec3 offset;
layout (location = 4) in vec3 speed;
out VS_OUT {
    vec3 normal;
    vec3 position;
} vs_out;

uniform mat4 model;
layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

void main() {
    vec3 d = normalize(speed);
    mat3 rot;
    if (d.z == 1 || d.z == -1) {
        rot = mat3(
            1, 0, 0,
            0, 0, d.z,
            0, -d.z, 0);
    } else {
        float c = sqrt(1 - d.z * d.z);
        rot = mat3(
            d.y / c, -d.x / c, 0,
            d.x, d.y, d.z,
            -d.x * d.z / c, - d.y * d.z / c, c);
    }
    vs_out.position = vec3(model * vec4(rot * position + offset, 1.0));
    vs_out.normal = transpose(inverse(mat3(model) * rot)) * normal;
    gl_Position = projection * view * vec4(vs_out.position, 1.0);
}