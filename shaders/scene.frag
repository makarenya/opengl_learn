# version 330 core

struct Material {
    sampler2D diffuse_map;
    vec4 diffuse_col;
    sampler2D specular_map;
    vec4 specular_col;
    sampler2D shiness_map;
    float shiness;
    float reflection;
    float refraction;
};

struct DirectionalLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float linear;
    float quadratic;
};

struct ProjectorLight {
    vec3 position;
    vec3 target;

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
} fs_in;

uniform vec3 viewPos;
uniform Material material;
uniform samplerCube skybox;
uniform sampler2D shadow;
uniform bool opaque;
layout (std140) uniform Lights {
    DirectionalLight directional;
    SpotLight spots[8];
    ProjectorLight projector;
    int spotCount;
};

out vec4 color;
const vec3 gamma = vec3(1.0 / 2.2);

vec3 CalcDirectionalLight(DirectionalLight light, vec3 norm, vec3 viewDir, vec3 diffuse, vec3 specular, float shiness);
vec3 CalcSpotLight(SpotLight light, vec3 norm, vec3 viewDir, vec3 diffuse, vec3 specular, float shiness);
vec3 CalcProjectorLight(ProjectorLight light, vec3 norm, vec3 viewDir, vec3 diffuse, vec3 specular, float shiness);

void main() {
    vec3 norm = normalize(fs_in.normal);
    vec3 viewDir = normalize(viewPos - fs_in.position);

    vec4 diffuse = material.diffuse_col.a > 0 ? material.diffuse_col : texture(material.diffuse_map, fs_in.coord);
    vec4 specular = material.specular_col.a > 0 ? material.specular_col : texture(material.specular_map, fs_in.coord);
    float shiness = material.shiness > 0 ? material.shiness : float(texture(material.shiness_map, fs_in.coord));

    vec3 result = CalcDirectionalLight(directional, norm, viewDir, diffuse.rgb, specular.rgb, shiness);
    for (int i = 0; i < spotCount; i++) {
        result += CalcSpotLight(spots[i], norm, viewDir, diffuse.rgb, specular.rgb, shiness);
    }
    result += CalcProjectorLight(projector, norm, viewDir, diffuse.rgb, specular.rgb, shiness);

    if (material.reflection > 0) {
        vec3 reflectDir = reflect(-viewDir, norm);
        vec3 sky = vec3(texture(skybox, reflectDir));
        float r = material.reflection;
        result = sky * r + result * (1.0 - r);
    }

    if (material.refraction > 0) {
        float ratio = 1.00 / 1.33;
        vec3 ref = vec3(texture(skybox, refract(-viewDir, norm, ratio)));
        result = ref * material.refraction + result * (1.0 - material.refraction);
    }

    if (opaque) {
        color = vec4(pow(result, gamma), diffuse.a);
    } else {
        color = vec4(pow(result, gamma), 1.0f);
    }
}

vec2 shadowTex = 0.5 / textureSize(shadow, 0);

float GetShadow() {
    vec3 coord = (fs_in.lightPos.xyz / fs_in.lightPos.w) * 0.5 + 0.5;
    if (coord.z > 1) return 1;
    float value = 1;
    for (int i = -3; i <= 3; i++) {
        for (int j = -3; j <= 3; j++) {
            if (texture(shadow, coord.xy + shadowTex * vec2(i, j)).r < coord.z) {
                value -= 1.0 / 49;
            }
        }
    }
    return value;
}

vec3 CalcDirectionalLight(DirectionalLight light, vec3 norm, vec3 viewDir, vec3 diffuse, vec3 specular, float shiness) {
    vec3 lightNorm = normalize(-light.direction);
    vec3 halfLight = normalize(lightNorm + viewDir);

    float diff = max(dot(norm, lightNorm), 0.0f);
    float spec = dot(norm, lightNorm) >= 0 ? pow(max(dot(norm, halfLight), 0.0f), shiness) : 0;
    float s = GetShadow();

    return diffuse * light.ambient +
        s * diffuse * diff * light.diffuse +
        s * specular.rgb * spec * light.specular;
}

vec3 CalcSpotLight(SpotLight light, vec3 norm, vec3 viewDir, vec3 diffuse, vec3 specular, float shiness) {
    vec3 lightNorm = normalize(light.position - fs_in.position);
    vec3 halfLight = normalize(lightNorm + viewDir);

    float distance = length(light.position - fs_in.position);
    float intensity = 1.0f / (1.0f + light.linear * distance + light.quadratic * distance * distance);

    float diff = max(dot(norm, lightNorm), 0.0f);
    float spec = dot(norm, lightNorm) >= 0 ? pow(max(dot(norm, halfLight), 0.0f), shiness) : 0;

    return (diffuse * light.ambient +
    diffuse * diff * light.diffuse +
    specular.rgb * spec * light.specular) * intensity;
}

vec3 CalcProjectorLight(ProjectorLight light, vec3 norm, vec3 viewDir, vec3 diffuse, vec3 specular, float shiness) {
    vec3 lightNorm = normalize(light.position - fs_in.position);
    vec3 halfLight = normalize(lightNorm + viewDir);
    vec3 lightDir = normalize(light.position - light.target);

    float distance = length(light.position - fs_in.position);
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