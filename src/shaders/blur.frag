#version 330 core
in vec2 fragmentCoord;
uniform sampler2D screenTexture;
uniform bool vertical;
uniform int size;
uniform float kernel[16];
uniform float threshold;
out vec4 color;

void main() {
    ivec2 sz = textureSize(screenTexture, 0);
    vec2 dir = vertical ? vec2(0.0f, 1.0f / sz.y) : vec2(1.0f / sz.x, 0.0f);
    vec3 result = vec3(0.0f);
    for (int i = -size; i <= size; i++) {
        vec3 tc = texture(screenTexture, fragmentCoord + dir * i).rgb;
        float brightness = dot(tc, vec3(0.2126, 0.7152, 0.0722));
        if (brightness > threshold)
            result += tc * kernel[abs(i)];
    }
    color = vec4(result, 1.0f);
}