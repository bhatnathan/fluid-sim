#version 330 core

// Output data
out float divergence;

uniform sampler2D vecField;
uniform float divergenceMod;

void main() {
	ivec2 fragCoord = ivec2(gl_FragCoord.xy);

	float wR = texelFetchOffset(vecField, fragCoord, 0, ivec2(1, 0)).x;
	float wT = texelFetchOffset(vecField, fragCoord, 0, ivec2(0, 1)).y;
	float wL = texelFetchOffset(vecField, fragCoord, 0, ivec2(-1, 0)).x;
	float wB = texelFetchOffset(vecField, fragCoord, 0, ivec2(0, -1)).y;

	divergence = divergenceMod * ((wR - wL) + (wT - wB));
}