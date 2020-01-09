#version 330 core

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

in VS_OUT {
    vec3 normal;
    vec3 position;
} fs_in;

out vec4 color;
uniform samplerCube skybox;
uniform vec3 viewPos;
layout (std140) uniform Lights {
    DirectionalLight directional;
    SpotLight spots[8];
    ProjectorLight projector;
    int spotCount;
};

vec3 CalcDirectionalLight(DirectionalLight light, vec3 norm, vec3 viewDir, vec3 diffuse, vec3 specular, float shiness);
vec3 CalcSpotLight(SpotLight light, vec3 norm, vec3 viewDir, vec3 diffuse, vec3 specular, float shiness);
vec3 CalcProjectorLight(ProjectorLight light, vec3 norm, vec3 viewDir, vec3 diffuse, vec3 specular, float shiness);


void main() {
    vec3 skyCoord = refract(normalize(vec3(fs_in.position) - viewPos), normalize(fs_in.normal), 1 / 1.2);
    vec3 diffuse = vec3(.5, .5, .6);
    vec3 norm = normalize(fs_in.normal);
    vec3 viewDir = normalize(viewPos - vec3(fs_in.position));
    vec3 specular = vec3(1, 1, 1);
    float shiness = 32;
    vec3 result = CalcDirectionalLight(directional, norm, viewDir, diffuse, specular, shiness);
    for (int i = 0; i < spotCount; i++) {
        result += CalcSpotLight(spots[i], norm, viewDir, diffuse, specular.rgb, shiness);
    }
    result += CalcProjectorLight(projector, norm, viewDir, diffuse, specular.rgb, shiness);
    color = vec4(result * .3 + vec3(texture(skybox, skyCoord)) * 0.7, 1.0);
}

vec3 CalcDirectionalLight(DirectionalLight light, vec3 norm, vec3 viewDir, vec3 diffuse, vec3 specular, float shiness) {
    vec3 lightNorm = normalize(-light.direction);
    vec3 halfLight = normalize(lightNorm + viewDir);

    float diff = max(dot(norm, lightNorm), 0.0f);
    float spec = pow(max(dot(norm, halfLight), 0.0f), shiness);

    return diffuse * light.ambient +
    diffuse * diff * light.diffuse +
    specular.rgb * spec * light.specular;
}

vec3 CalcSpotLight(SpotLight light, vec3 norm, vec3 viewDir, vec3 diffuse, vec3 specular, float shiness) {
    vec3 lightNorm = normalize(light.position - fs_in.position);
    vec3 halfLight = normalize(lightNorm + viewDir);

    float distance = length(light.position - fs_in.position);
    float intensity = 1.0f / (1.0f + light.linear * distance + light.quadratic * distance * distance);

    float diff = max(dot(norm, lightNorm), 0.0f);
    float spec = pow(max(dot(norm, halfLight), 0.0f), shiness);

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
    float spec = pow(max(dot(norm, halfLight), 0.0f), shiness);

    return (diffuse * light.ambient +
    diffuse * diff * light.diffuse +
    specular.rgb * spec * light.specular) * intensity;
}