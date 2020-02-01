#version 330 core
in vec2 fragmentCoord;
uniform sampler2D screenTexture;
uniform sampler2D depthTexture;
out vec4 color;

void main() {
    gl_FragDepth = texture(depthTexture, fragmentCoord).r;
    color = texture(screenTexture, fragmentCoord);
}