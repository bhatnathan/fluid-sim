#version 330 core

// Output Data
out int vInstance;

// Input Data
in vec4 position;

void main() {
    gl_Position = position;
    vInstance = gl_InstanceID;
}