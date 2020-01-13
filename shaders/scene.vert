# version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 norm;
layout (location = 2) in vec2 coord;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;

uniform mat4 model;
uniform mat3 normModel;
uniform mat4 lightMatrix;

out VS_OUT {
    vec3 normal;
    vec3 position;
    vec4 lightPos;
    vec2 coord;
    vec3 tangent;
    vec3 bitangent;
} vs_out;

void main() {

    gl_Position = model * vec4(position, 1.0f);
    vs_out.position = vec3(model * vec4(position, 1.0f));
    vs_out.lightPos = lightMatrix * model * vec4(position, 1.0);
    vs_out.coord = coord;
    vs_out.normal = normModel * norm;
    vs_out.tangent = normModel * tangent;
    vs_out.bitangent = normModel * bitangent;
}
