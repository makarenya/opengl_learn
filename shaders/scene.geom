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
    vec2 coord;
} gs_in[];

out GS_OUT {
    vec3 normal;
    vec3 position;
    vec2 coord;
} gs_out;

uniform float explosion;

void main() {
    vec4 v0 = gl_in[0].gl_Position;
    vec4 v1 = gl_in[1].gl_Position;
    vec4 v2 = gl_in[2].gl_Position;
    vec4 offset = vec4(normalize(cross(vec3(v2 - v1), vec3(v0 - v1))), 0.0) * explosion;

    gl_Position = projection * view * (v0 + offset);
    gs_out.normal = gs_in[0].normal;
    gs_out.position = gs_in[0].position;
    gs_out.coord = gs_in[0].coord;
    EmitVertex();

    gl_Position = projection * view * (v1 + offset);
    gs_out.normal = gs_in[1].normal;
    gs_out.position = gs_in[1].position;
    gs_out.coord = gs_in[1].coord;
    EmitVertex();

    gl_Position = projection * view * (v2 + offset);
    gs_out.normal = gs_in[2].normal;
    gs_out.position = gs_in[2].position;
    gs_out.coord = gs_in[2].coord;
    EmitVertex();

    EndPrimitive();
}
