# version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 norm;
layout (location = 2) in vec2 coord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 fragmentNormal;
out vec3 fragmentPos;
out vec2 fragmentCoord;

void main() {
    gl_Position = projection * view * model * vec4(position, 1.0f);
    fragmentPos = vec3(model * vec4(position, 1.0f));
    fragmentNormal = mat3(transpose(inverse(model))) * norm;
    fragmentCoord = coord;
}
