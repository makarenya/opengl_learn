#version 330 core

in vec2 fragmentCoord;
uniform bool opacity;
uniform sampler2D diffuse;

void main() {
    if (opacity) {
        if (texture(diffuse, fragmentCoord).a < .2) discard;
    }
}