#version 330 core
in vec2 fragmentCoord;
uniform vec4 borderColor;
uniform sampler2D screenTexture;
uniform sampler2D depthTexture;
out vec4 color;

const int size = 1;

void main() {
    if (texture(screenTexture, fragmentCoord).r > 0) discard;
    ivec2 sz = textureSize(screenTexture, 0);
    vec2 dpi = vec2(1.0 / sz.x, 1.0 / sz.y);

    float kernel[size * size * 4 + 4 * size + 1] = float[] (
    0.2149, 0.6777, 0.2149,
    0.6777, 1.0000, 0.6777,
    0.2149, 0.6777, 0.2149
    );

    float result = 0;
    float depth = 1.0f;
    for (int y = 0; y < size * 2 + 1; y++) {
        for (int x = 0; x < size * 2 + 1; x++) {
            vec2 offset = vec2(x - size, y - size) * dpi;
            result = max(result, texture(screenTexture, fragmentCoord + offset).r * kernel[y * (2 * size + 1) + x]);
            depth = min(depth, texture(depthTexture, fragmentCoord + offset).r);
        }
    }
    gl_FragDepth = depth;
    color = vec4(borderColor.rgb, result * borderColor.a);
}