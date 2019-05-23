#version 330 core

// Output Data
out float gLayer;
layout(triangle_strip, max_vertices = 3) out;

// Input Data
layout(triangles) in;
in int vInstance[3];
 
void main() {
    gl_Layer = vInstance[0];
    gLayer = float(gl_Layer) + 0.5;
    gl_Position = gl_in[0].gl_Position;
    EmitVertex();
    gl_Position = gl_in[1].gl_Position;
    EmitVertex();
    gl_Position = gl_in[2].gl_Position;
    EmitVertex();
    EndPrimitive();
}