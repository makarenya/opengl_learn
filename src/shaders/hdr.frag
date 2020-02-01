#version 330 core
in vec2 fragmentCoord;
uniform sampler2D screenTexture;
uniform sampler2D depthTexture;
uniform float exposure;
out vec4 color;

const vec3 gamma = vec3(1.0 / 2.2);

void main() {
    gl_FragDepth = texture(depthTexture, fragmentCoord).r;
    vec3 c = texture(screenTexture, fragmentCoord).rgb;
    vec3 m = vec3(1.0f) - exp(-c * exposure);
    color = vec4(pow(m, gamma), 1.0f);
}