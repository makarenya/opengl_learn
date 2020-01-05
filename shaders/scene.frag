# version 330 core

struct Material {
    sampler2D diffuse_map;
    vec4 diffuse_col;
    sampler2D specular_map;
    vec4 specular_col;
    sampler2D shiness_map;
    float shiness;
    samplerCube skybox;
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
};

in vec3 fragmentNormal;
in vec3 fragmentPos;
in vec2 fragmentCoord;

#define SPOT_LIGHTS 8

uniform vec3 viewPos;
uniform int spotCount;
uniform Material material;
uniform DirectionalLight directional;
uniform SpotLight spots[SPOT_LIGHTS];
uniform ProjectorLight projector;
uniform bool canDiscard;

out vec4 color;

vec3 CalcDirectionalLight(DirectionalLight light, vec3 norm, vec3 viewDir, vec3 diffuse, vec3 specular, float shiness);
vec3 CalcSpotLight(SpotLight light, vec3 norm, vec3 viewDir, vec3 diffuse, vec3 specular, float shiness);
vec3 CalcProjectorLight(ProjectorLight light, vec3 norm, vec3 viewDir, vec3 diffuse, vec3 specular, float shiness);

void main() {
    vec3 norm = normalize(fragmentNormal);
    vec3 viewDir = normalize(viewPos - fragmentPos);

    vec4 diffuse = material.diffuse_col.a > 0 ? material.diffuse_col : texture(material.diffuse_map, fragmentCoord);
    vec4 specular = material.specular_col.a > 0 ? material.specular_col : texture(material.specular_map, fragmentCoord);
    float shiness = material.shiness > 0 ? material.shiness : float(texture(material.shiness_map, fragmentCoord));

    vec3 result = CalcDirectionalLight(directional, norm, viewDir, diffuse.rgb, specular.rgb * specular.a, shiness);
    for (int i = 0; i < spotCount; i++) {
        result += CalcSpotLight(spots[i], norm, viewDir, diffuse.rgb, specular.rgb * specular.a, shiness);
    }
    result += CalcProjectorLight(projector, norm, viewDir, diffuse.rgb, specular.rgb * specular.a, shiness);

    if (material.reflection > 0) {
        vec3 reflectDir = reflect(-viewDir, norm);
        vec3 sky = vec3(texture(material.skybox, reflectDir));
        float r = material.reflection * (specular.r + specular.g + specular.b) / 3;
        result = sky * r + result * (1.0 - r);
    }

    if (material.refraction > 0) {
        float ratio = 1.00 / 1.33;
        vec3 ref = vec3(texture(material.skybox, refract(-viewDir, norm, ratio)));
        result = ref * material.refraction + result * (1.0 - material.refraction);
    }

    if (canDiscard) {
        color = vec4(result, diffuse.a);
    } else {
        color = vec4(result, 1.0f);
    }
}

vec3 CalcDirectionalLight(DirectionalLight light, vec3 norm, vec3 viewDir, vec3 diffuse, vec3 specular, float shiness) {
    vec3 lightNorm = normalize(-light.direction);
    vec3 reflectDir = reflect(-lightNorm, norm);

    float diff = max(dot(norm, lightNorm), 0.0f);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), shiness);

    return diffuse * light.ambient +
    diffuse * diff * light.diffuse +
    specular.rgb * spec * light.specular;
}

vec3 CalcSpotLight(SpotLight light, vec3 norm, vec3 viewDir, vec3 diffuse, vec3 specular, float shiness) {
    vec3 lightNorm = normalize(light.position - fragmentPos);
    vec3 reflectDir = reflect(-lightNorm, norm);

    float distance = length(light.position - fragmentPos);
    float intensity = 1.0f / (1.0f + light.linear * distance + light.quadratic * distance * distance);

    float diff = max(dot(norm, lightNorm), 0.0f);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), shiness);

    return (diffuse * light.ambient +
    diffuse * diff * light.diffuse +
    specular.rgb * spec * light.specular) * intensity;
}

vec3 CalcProjectorLight(ProjectorLight light, vec3 norm, vec3 viewDir, vec3 diffuse, vec3 specular, float shiness) {
    vec3 lightNorm = normalize(light.position - fragmentPos);
    vec3 reflectDir = reflect(-lightNorm, norm);
    vec3 lightDir = normalize(light.position - light.target);

    float intensity = clamp(
    (dot(lightNorm, lightDir) - light.outerCutoff) /
    (light.innerCutoff - light.outerCutoff),
    0.0f, 1.0f);

    float diff = max(dot(norm, lightNorm), 0.0f);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), shiness);

    return (diffuse * light.ambient +
    diffuse * diff * light.diffuse +
    specular.rgb * spec * light.specular) * intensity;
}