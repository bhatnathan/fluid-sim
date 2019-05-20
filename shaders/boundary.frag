#version 330 core

//Input Data
in vec2 coords;
in vec2 offset;

// Output data
out vec4 targetOut;

uniform float scale;
uniform sampler2D target;

void main() {
	targetOut = scale * texture2D(target, coords + offset);
}