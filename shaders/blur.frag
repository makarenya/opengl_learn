#version 330 core
in vec2 fragmentCoord;
uniform sampler2D screenTexture;
uniform sampler2D depthTexture;
out vec4 color;

const int size = 3;

void main() {
    ivec2 sz = textureSize(screenTexture, 0);
    vec2 dpi = vec2(1.0 / sz.x, 1.0 / sz.y);

    float kernel[size * size * 4 + 4 * size + 1] = float[] (
        0.0005, 0.0023, 0.0054, 0.0072, 0.0054, 0.0023, 0.0005,
        0.0023, 0.0096, 0.0225, 0.0299, 0.0225, 0.0096, 0.0023,
        0.0054, 0.0225, 0.0521, 0.0684, 0.0521, 0.0225, 0.0054,
        0.0072, 0.0299, 0.0684, 0.0869, 0.0684, 0.0299, 0.0072,
        0.0054, 0.0225, 0.0521, 0.0684, 0.0521, 0.0225, 0.0054,
        0.0023, 0.0096, 0.0225, 0.0299, 0.0225, 0.0096, 0.0023,
        0.0005, 0.0023, 0.0054, 0.0072, 0.0054, 0.0023, 0.0005
    );

    vec4 result = vec4(0);
    float depth = 1.0f;
    for (int y = 0; y < size * 2 + 1; y++) {
        for (int x = 0; x < size * 2 + 1; x++) {
            vec2 offset = vec2(x - size, y - size) * dpi;
            result += texture(screenTexture, fragmentCoord + offset) * kernel[y * (2 * size + 1) + x];
            depth = min(depth, texture(depthTexture, fragmentCoord + offset).r);
        }
    }
    gl_FragDepth = depth;
    color = result;
}