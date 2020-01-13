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
    vec3 tangent;
    vec3 bitangent;
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
    vec4 offset = vec4(normalize(cross(v2 - v1, v0 - v1)), 0.0) * explosion;
    for (int i = 0; i < 3; ++i) {
        gl_Position = projection * view * (gl_in[i].gl_Position + offset);
        gs_out.lightPos = gs_in[i].lightPos;
        gs_out.coord = gs_in[i].coord;
        if (gs_in[i].tangent != vec3(0) || gs_in[i].bitangent != vec3(0)) {
            mat3 tbn = mat3(
                normalize(gs_in[i].tangent),
                normalize(gs_in[i].bitangent),
                normalize(gs_in[i].normal)
            );
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
        } else {
            gs_out.tbn = mat3(1, 0, 0, 0, 1, 0, 0, 0, 1);
            gs_out.normal = gs_in[i].normal;
            gs_out.position = gs_in[i].position;
            gs_out.directional = directional;
            gs_out.spots = spots;
            gs_out.projector = projector;
            gs_out.viewPos = viewPos;
        }
        EmitVertex();
    }

    EndPrimitive();
}
