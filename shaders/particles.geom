#version 330 core
layout (points) in;
layout (triangle_strip, max_vertices = 5) out;
in VS_OUT {
    vec3 speed;
} gs_in[];
out vec3 fragmentColor;

void build_house(vec4 position, vec3 speed) {
    fragmentColor = vec3(0, 0, 1);
    mat4 rot = mat4(
        speed.y, -speed.x, 0, 0,
        speed.x, speed.y, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    );
    gl_Position = position + rot * vec4(-0.2, -0.2, 0.0, 0.0);
    EmitVertex();

    gl_Position = position + rot * vec4(0.2, -0.2, 0.0, 0.0);
    EmitVertex();

    gl_Position = position + rot * vec4(-0.2, 0.2, 0.0, 0.0);
    EmitVertex();

    gl_Position = position + rot * vec4(0.2, 0.2, 0.0, 0.0);
    EmitVertex();

    gl_Position = position + rot * vec4(0.0, 0.4, 0.0, 0.0);
    fragmentColor = vec3(1, 1, 1);
    EmitVertex();
    EndPrimitive();
}

void main() {
    build_house(gl_in[0].gl_Position, normalize(gs_in[0].speed));
}