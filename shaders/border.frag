#version 330 core
in vec2 fragmentCoord;

uniform vec4 borderColor;
uniform sampler2D screenTexture;
uniform sampler2D depthTexture;
uniform int size;
uniform float kernel[49];
uniform float inner[49];

out vec4 color;

void main() {
    ivec2 sz = textureSize(screenTexture, 0);
    vec2 dpi = vec2(1.0 / sz.x, 1.0 / sz.y);
    float result = 0;
    float iresult = 0;
    float depth = 1.0;
    for (int y = 0; y < size * 2 + 1; y++) {
        for (int x = 0; x < size * 2 + 1; x++) {
            vec2 offset = vec2(x - size, y - size) * dpi;
            float value = texture(screenTexture, fragmentCoord + offset).r;
            result = max(result, value * kernel[y * (2 * size + 1) + x]);
            iresult = max(iresult, value * inner[y * (2 * size + 1) + x]);
            depth = min(depth, texture(depthTexture, fragmentCoord + offset).r);
        }
    }
    result *= (1 - iresult);
    if (result == 0) discard;
    gl_FragDepth = depth;
    color = vec4(borderColor.rgb, result * borderColor.a);
}