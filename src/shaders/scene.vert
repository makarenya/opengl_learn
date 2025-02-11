# version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 coord;

uniform mat4 model;
uniform mat3 norm;
uniform mat4 light;

out VS_OUT {
    vec3 normal;
    vec3 position;
    vec4 lightPos;
    vec2 coord;
} vs_out;

void main() {
    vec4 pos = vec4(position, 1.0f);
    gl_Position = model * pos;
    vs_out.position = vec3(gl_Position);
    vs_out.lightPos = light * pos;
    vs_out.coord = coord;
    vs_out.normal = norm * normal;
}
