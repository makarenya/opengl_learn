#version 330 core

in GS_OUT {
    vec3 position;
    vec2 coord;
} fs_in;

uniform bool opacity;
uniform sampler2D diffuse;
uniform vec3 lightPos;
uniform bool direct;

void main() {
    if (opacity) {
        if (texture(diffuse, fs_in.coord).a < .2) discard;
    }
    if (direct) {
        gl_FragDepth = gl_FragCoord.z;
    } else {
        gl_FragDepth = length(fs_in.position - lightPos) / 100.0;
    }
}