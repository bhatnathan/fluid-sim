#version 330 core

//From tutorial

// Output Data
layout(triangle_strip, max_vertices = 24) out;

// Input Data
layout(points) in;
in vec4 vertexPosition[1];

uniform mat4 mvp;

vec4 ndcCube[8]; // Normalized device coordinate of cube corner
ivec4 faces[6];  // Vertex indices of the cube faces

void emitVert(int vert) {
    gl_Position = ndcCube[vert];
    EmitVertex();
}

void emitFace(int face) {
    emitVert(faces[face][1]); emitVert(faces[face][0]);
    emitVert(faces[face][3]); emitVert(faces[face][2]);
    EndPrimitive();
}

void main() {
    faces[0] = ivec4(0,1,3,2); faces[1] = ivec4(5,4,6,7);
    faces[2] = ivec4(4,5,0,1); faces[3] = ivec4(3,2,7,6);
    faces[4] = ivec4(0,3,4,7); faces[5] = ivec4(2,1,6,5);

    vec4 pos = vertexPosition[0];
    vec4 xAxis = vec4(1,0,0,0);
    vec4 yAxis = vec4(0,1,0,0);
    vec4 zAxis = vec4(0,0,1,0);

	vec4 objCube[8]; // Object space coordinate of cube corner
    objCube[0] = pos + xAxis + yAxis + zAxis; objCube[1] = pos + xAxis - yAxis + zAxis;
    objCube[2] = pos - xAxis - yAxis + zAxis; objCube[3] = pos - xAxis + yAxis + zAxis;
    objCube[4] = pos + xAxis + yAxis - zAxis; objCube[5] = pos + xAxis - yAxis - zAxis;
    objCube[6] = pos - xAxis - yAxis - zAxis; objCube[7] = pos - xAxis + yAxis - zAxis;

    // Transform the corners of the box:
    for (int vert = 0; vert < 8; vert++)
        ndcCube[vert] = mvp * objCube[vert];

    // Emit the six faces:
    for (int face = 0; face < 6; face++)
        emitFace(face);
}