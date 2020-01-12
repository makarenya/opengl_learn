#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;
layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

in VS_OUT {
    vec3 normal;
    vec3 position;
    vec4 lightPos;
    vec2 coord;
} gs_in[];

out GS_OUT {
    vec3 normal;
    vec3 position;
    vec4 lightPos;
    vec2 coord;
} gs_out;

uniform float explosion;

void main() {
    vec4 v0 = gl_in[0].gl_Position;
    vec4 v1 = gl_in[1].gl_Position;
    vec4 v2 = gl_in[2].gl_Position;
    vec4 offset = vec4(normalize(cross(vec3(v2 - v1), vec3(v0 - v1))), 0.0) * explosion;

    for (int i = 0; i < 3; ++i) {
        gl_Position = projection * (gl_in[i].gl_Position + offset);
        gs_out.normal = gs_in[i].normal;
        gs_out.position = gs_in[i].position;
        gs_out.lightPos = gs_in[i].lightPos;
        gs_out.coord = gs_in[i].coord;
        EmitVertex();
    }

    EndPrimitive();
}
