#version 330 core

in VS_OUT {
    vec3 normal;
    vec4 position;
} fs_in;

out vec4 color;
uniform samplerCube skybox;
uniform vec3 viewPos;

void main() {
    vec3 sky = reflect(normalize(vec3(fs_in.position) - viewPos), normalize(fs_in.normal));
    color = texture(skybox, sky);
}