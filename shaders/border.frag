#version 330 core
in vec2 fragmentCoord;
uniform vec4 borderColor;
uniform sampler2D screenTexture;
uniform sampler2D depthTexture;
out vec4 color;

const int size = 2;

void main() {
    if (texture(screenTexture, fragmentCoord).r > 0) discard;
    ivec2 sz = textureSize(screenTexture, 0);
    vec2 dpi = vec2(1.0 / sz.x, 1.0 / sz.y);

    float kernel[size * size * 4 + 4 * size + 1] = float[] (
        0.0000, 0.2149, 0.4545, 0.2149, 0.0000,
        0.2149, 0.9917, 1.0000, 0.9917, 0.2149,
        0.4545, 1.0000, 1.0000, 1.0000, 0.4545,
        0.2149, 0.9917, 1.0000, 0.9917, 0.2149,
        0.0000, 0.2149, 0.4545, 0.2149, 0.0000
    );

    float result = 0;
    float depth = .99;
    for (int y = 0; y < size * 2 + 1; y++) {
        for (int x = 0; x < size * 2 + 1; x++) {
            vec2 offset = vec2(x - size, y - size) * dpi;
            result = max(result, texture(screenTexture, fragmentCoord + offset).r * kernel[y * (2 * size + 1) + x]);
            depth = min(depth, texture(depthTexture, fragmentCoord + offset).r);
        }
    }
    if (result == 0) discard;
    gl_FragDepth = depth;
    color = vec4(borderColor.rgb, result * borderColor.a);
}