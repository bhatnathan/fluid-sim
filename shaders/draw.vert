#version 330 core

// Output Data
out vec4 vertexPosition;

// Input Data
in vec4 position;

uniform mat4 mvp;

void main() {
    gl_Position = mvp * position;
    vertexPosition = position;
}