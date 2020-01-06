# version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 norm;
layout (location = 2) in vec2 coord;

uniform mat4 model;

out VS_OUT {
    vec3 normal;
    vec3 position;
    vec2 coord;
} vs_out;

void main() {
    gl_Position = model * vec4(position, 1.0f);
    vs_out.position = vec3(model * vec4(position, 1.0f));
    vs_out.normal = mat3(transpose(inverse(model))) * norm;
    vs_out.coord = coord;
}
