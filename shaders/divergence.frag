#version 330 core

// Output data
out float div;

uniform sampler2D vecField;
uniform float divergenceMod;

void main() {
	ivec2 coords = ivec2(gl_FragCoord.xy);

	float wL = texelFetchOffset(vecField, coords, 0, ivec2(-1, 0)).x;
	float wR = texelFetchOffset(vecField, coords, 0, ivec2(1, 0)).x;
	float wB = texelFetchOffset(vecField, coords, 0, ivec2(0, -1)).y;
	float wT = texelFetchOffset(vecField, coords, 0, ivec2(0, 1)).y;

	div = divergenceMod * ((wR - wL) + (wT - wB));
}