#version 330 core

// Output data
out float divergence;

uniform sampler3D vecField;

uniform float divergenceMod;

in float gLayer;

void main() {
	ivec3 fragCoord = ivec3(gl_FragCoord.xy, gLayer);

	float wE = texelFetchOffset(vecField, fragCoord, 0, ivec3(1, 0, 0)).x;
	float wN = texelFetchOffset(vecField, fragCoord, 0, ivec3(0, 1, 0)).y;
	float wU = texelFetchOffset(vecField, fragCoord, 0, ivec3(0, 0, 1)).z;
	float wW = texelFetchOffset(vecField, fragCoord, 0, ivec3(-1, 0, 0)).x;
	float wS = texelFetchOffset(vecField, fragCoord, 0, ivec3(0, -1, 0)).y;
	float wD = texelFetchOffset(vecField, fragCoord, 0, ivec3(0, 0, -1)).z;

	divergence = divergenceMod * ((wE - wW) + (wN - wS) + (wU - wD));
}