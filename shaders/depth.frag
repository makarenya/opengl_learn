#version 330 core
in vec2 fragmentCoord;
uniform sampler2D depthTexture;
uniform vec2 perspective;
out vec4 color;

void main() {
    float value = texture(depthTexture, fragmentCoord).r;
    if (perspective == vec2(0)) {
        color = vec4(value, value, value, 1.0);
    } else {
        float near = perspective[0];
        float far = perspective[1];
        color = vec4(vec3((2.0 * near * far) / (far + near - (value * 2.0 - 1.0) * (far - near))), 1.0);
    }
}
