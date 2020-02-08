#version 330 core

struct ProjectorLightPos {
    vec3 position;
    vec3 target;
};

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;
layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

layout (std140) uniform LightsPos {
    vec3 directional;
    vec3 spots[4];
    ProjectorLightPos projector;
};

in VS_OUT {
    vec3 normal;
    vec3 position;
    vec4 lightPos;
    vec2 coord;
} gs_in[];

out GS_OUT {
    mat3 tbn;
    vec3 normal;
    vec3 position;
    vec4 lightPos;
    vec2 coord;
    vec3 directional;
    vec3 spots[4];
    ProjectorLightPos projector;
    vec3 viewPos;
} gs_out;

uniform float explosion;
uniform vec3 viewPos;

void main() {
    vec3 v0 = vec3(gl_in[0].gl_Position);
    vec3 v1 = vec3(gl_in[1].gl_Position);
    vec3 v2 = vec3(gl_in[2].gl_Position);
    vec2 c0 = gs_in[0].coord;
    vec2 c1 = gs_in[1].coord;
    vec2 c2 = gs_in[2].coord;
    vec3 dv1 = v2 - v1;
    vec3 dv2 = v0 - v1;
    vec2 dc1 = c2 - c1;
    vec2 dc2 = c0 - c1;
    vec3 tg = normalize(vec3(-dc1.y*dv2.x + dc2.y*dv1.x, -dc1.y*dv2.y + dc2.y*dv1.y, -dc1.y*dv2.z + dc2.y*dv1.z) / (dc1.x * dc2.y - dc1.y * dc2.x));
    vec3 btg = normalize(vec3(dc1.x*dv2.x - dc2.x*dv1.x, dc1.x*dv2.y - dc2.x*dv1.y, dc1.x*dv2.z - dc2.x*dv1.z) / (dc1.x * dc2.y - dc1.y * dc2.x));

    vec4 offset = vec4(normalize(cross(v2 - v1, v0 - v1)), 0.0) * explosion;
    for (int i = 0; i < 3; ++i) {
        gl_Position = projection * view * (gl_in[i].gl_Position + offset);
        gs_out.lightPos = gs_in[i].lightPos;
        gs_out.coord = gs_in[i].coord;
        vec3 n = normalize(gs_in[i].normal);
        vec3 t = normalize(tg - dot(tg, n) * n);
        vec3 b = normalize(btg - dot(btg, n) * n);
        mat3 tbn = mat3(t, b, n);
        mat3 itbn = transpose(tbn);
        gs_out.tbn = tbn;
        gs_out.normal = itbn * gs_in[i].normal;
        gs_out.position = itbn * gs_in[i].position;
        gs_out.directional = itbn * directional;
        for (int j = 0; j < 4; ++j) {
            gs_out.spots[j] = itbn * spots[j];
        }
        gs_out.projector.position = itbn * projector.position;
        gs_out.projector.target = itbn * projector.target;
        gs_out.viewPos = itbn * viewPos;
        EmitVertex();
    }

    EndPrimitive();
}
