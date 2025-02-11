# version 330 core

struct Material {
    sampler2D diffuse_map;
    bool has_diffuse_map;
    vec4 diffuse_col;
    sampler2D specular_map;
    bool has_specular_map;
    vec4 specular_col;
    float shiness;
    sampler2D normal_map;
    bool has_normal_map;
    sampler2D height_map;
    bool has_height_map;
};

struct DirectionalLight {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float linear;
    float quadratic;
};

struct ProjectorLightPos {
    vec3 position;
    vec3 target;
};

struct ProjectorLight {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float innerCutoff;
    float outerCutoff;
    float linear;
    float quadratic;
};

in GS_OUT {
    vec3 normal;
    vec3 position;
    vec4 lightPos;
    vec2 coord;
    vec3 directional;
    vec3 spots[4];
    vec3 shadows[4];
    ProjectorLightPos projector;
    vec3 viewPos;
} fs_in;

uniform Material material;
uniform sampler2D shadow;
uniform samplerCube spotShadow;
uniform samplerCube spotShadow2;
uniform bool opaque;
uniform bool useMap;
layout (std140) uniform Lights {
    DirectionalLight directional;
    SpotLight spots[4];
    ProjectorLight projector;
    int spotCount;
};


out vec4 color;
const float depth = 0.01;

vec3 CalcDirectionalLight(DirectionalLight light, vec3 dir, vec3 norm, vec3 viewDir, vec3 diffuse, vec3 specular, float shiness);
vec3 CalcSpotLight(SpotLight light, vec3 pos, int i, vec3 norm, vec3 viewDir, vec3 diffuse, vec3 specular, float shiness);
vec3 CalcProjectorLight(ProjectorLight light, ProjectorLightPos pos, vec3 norm, vec3 viewDir, vec3 diffuse, vec3 specular, float shiness);

void main() {
    vec2 coord = fs_in.coord;
    vec3 viewDir = normalize(fs_in.viewPos - fs_in.position);
    if (material.has_height_map && useMap) {
        vec2 toBottom = -depth * viewDir.xy / viewDir.z;
        float c = 0;
        float s = 0.5;
        for (int i = 0; i < 5; ++i) {
            float cd = texture(material.height_map, fs_in.coord + toBottom * (c + s)).r;
            if (cd > c + s)
                c += s;
            s /= 2;
        }
        coord += toBottom * c;
    }
    vec3 norm = normalize(material.has_normal_map ? vec3(texture(material.normal_map, coord)) : fs_in.normal);

    vec4 diffuse = material.has_diffuse_map ? texture(material.diffuse_map, coord) : material.diffuse_col;
    vec4 specular = material.has_specular_map ? texture(material.specular_map, coord) : material.specular_col;
    float shiness = material.shiness;

    vec3 result = CalcDirectionalLight(directional, fs_in.directional, norm, viewDir, diffuse.rgb, specular.rgb, shiness);
    for (int i = 0; i < spotCount; i++) {
        result += CalcSpotLight(spots[i], fs_in.spots[i], i, norm, viewDir, diffuse.rgb, specular.rgb, shiness);
    }
    result += CalcProjectorLight(projector, fs_in.projector, norm, viewDir, diffuse.rgb, specular.rgb, shiness);

    if (opaque) {
        color = vec4(result, diffuse.a);
    } else {
        color = vec4(result, 1.0f);
    }
}

vec2 shadowTex = 0.5 / textureSize(shadow, 0);

vec3 CalcDirectionalLight(DirectionalLight light, vec3 dir, vec3 norm, vec3 viewDir, vec3 diffuse, vec3 specular, float shiness) {
    vec3 lightNorm = normalize(-dir);
    vec3 halfLight = normalize(lightNorm + viewDir);

    float diff = max(dot(norm, lightNorm), 0.0f);
    float spec = dot(norm, lightNorm) >= 0 ? pow(max(dot(norm, halfLight), 0.0f), shiness) : 0;
    float s = 1.0f;
    vec3 scoord = fs_in.lightPos.xyz * 0.5 + 0.5;
    if (fs_in.lightPos.z <= 1) {
        for (int i = -3; i <= 3; i++) {
            for (int j = -3; j <= 3; j++) {
                if (texture(shadow, scoord.xy + shadowTex * vec2(i, j)).r < scoord.z + 0.001) {
                    s -= 1.0 / 49;
                }
            }
        }
    }

    return diffuse * light.ambient +
    s * diffuse * diff * light.diffuse +
    s * specular.rgb * spec * light.specular;
}

vec3 sampleOffsetDirections[9] = vec3[] (
    vec3(0, 0, 0),
    vec3(1, 1, 1), vec3(1, 1, -1), vec3(1, -1, 1), vec3(1, -1, -1),
    vec3(-1, 1, 1), vec3(-1, 1, -1), vec3(-1, -1, 1), vec3(-1, -1, -1)
);

vec3 CalcSpotLight(SpotLight light, vec3 pos, int i, vec3 norm, vec3 viewDir, vec3 diffuse, vec3 specular, float shiness) {
    vec3 lightNorm = normalize(pos - fs_in.position);
    vec3 halfLight = normalize(lightNorm + viewDir);

    float distance = length(pos - fs_in.position);
    float intensity = 1.0f / (1.0f + light.linear * distance + light.quadratic * distance * distance);

    float diff = max(dot(norm, lightNorm), 0.0f);
    float spec = dot(norm, lightNorm) >= 0 ? pow(max(dot(norm, halfLight), 0.0f), shiness) : 0;
    float s = 1.0f;
    vec3 fragFromLight = fs_in.shadows[i];
    float dist = length(fragFromLight);
    if (i == 1) {
        for (int j = 0; j < 9; j++) {
            if (texture(spotShadow, fragFromLight + sampleOffsetDirections[j] * dist * 0.003).r * 100 < dist + .04)
                s -= 1.0f / 9.0f;
        }
    } else if (i == 2) {
        for (int j = 0; j < 9; j++) {
            if (texture(spotShadow2, fragFromLight + sampleOffsetDirections[j] * dist * 0.003).r * 100 < dist + .04)
                s -= 1.0f / 9.0f;
        }
    }

    return (diffuse * light.ambient +
        s * diffuse * diff * light.diffuse +
        s * specular.rgb * spec * light.specular) * intensity;
}

vec3 CalcProjectorLight(ProjectorLight light, ProjectorLightPos pos, vec3 norm, vec3 viewDir, vec3 diffuse, vec3 specular, float shiness) {
    vec3 lightNorm = normalize(pos.position - fs_in.position);
    vec3 halfLight = normalize(lightNorm + viewDir);
    vec3 lightDir = normalize(pos.position - pos.target);

    float distance = length(pos.position - fs_in.position);
    float intensity = 1.0f / (1.0f + light.linear * distance + light.quadratic * distance * distance) * clamp(
    (dot(lightNorm, lightDir) - light.outerCutoff) /
    (light.innerCutoff - light.outerCutoff),
    0.0f, 1.0f);

    float diff = max(dot(norm, lightNorm), 0.0f);
    float spec = dot(norm, lightNorm) >= 0 ? pow(max(dot(norm, halfLight), 0.0f), shiness) : 0;

    return (diffuse * light.ambient +
    diffuse * diff * light.diffuse +
    specular.rgb * spec * light.specular) * intensity;
}
