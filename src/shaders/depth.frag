#version 330 core
in vec2 fragmentCoord;
uniform samplerCube depthTexture;
out vec4 color;

void main() {
    float value = texture(depthTexture, vec3(fragmentCoord.x * 2 - 1, fragmentCoord.y * 2 - 1, -0.5)).r;
    color = vec4(value, value, value, 1.0);
}
