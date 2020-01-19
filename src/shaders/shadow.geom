#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

in VS_OUT {
    vec3 position;
    vec2 coord;
} gs_in[];

out GS_OUT {
    vec3 position;
    vec2 coord;
} gs_out;

uniform mat4 lightMatrices[6];
uniform bool direct;

void main() {
    if (direct) {
        for (int i = 0; i < 3; i++) {
            gl_Position = lightMatrices[0] * gl_in[i].gl_Position;
            gs_out.position = gs_in[i].position;
            gs_out.coord = gs_in[i].coord;
            EmitVertex();
        }
        EndPrimitive();
    } else {
        for (int l = 0; l < 6; l++) {
            gl_Layer = l;
            for (int i = 0; i < 3; i++) {
                gl_Position = lightMatrices[l] * gl_in[i].gl_Position;
                gs_out.position = gs_in[i].position;
                gs_out.coord = gs_in[i].coord;
                EmitVertex();
            }
            EndPrimitive();
        }
    }
}