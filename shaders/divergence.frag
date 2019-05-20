#version 330 core

//Input Data
in vec2 coords;

// Output data
out float div;

uniform float rdx;
uniform sampler2D vecField;

void main() {
	float wL = texture2D(vecField, coords - vec2(1, 0)).x;
	float wR = texture2D(vecField, coords + vec2(1, 0)).x;
	float wB = texture2D(vecField, coords - vec2(0, 1)).y;
	float wT = texture2D(vecField, coords + vec2(0, 1)).y;
	div = rdx * ((wR - wL) + (wT - wB));
}