#version 330 core

// Output Data
out vec3 newVelocity;

// Input Data
in float gLayer;

uniform sampler3D velocity;
uniform sampler3D pressure;

uniform vec2 screenSize;
uniform float gradScale;

void main() {
	ivec3 fragCoord = ivec3(gl_FragCoord.xy, gLayer);

	float pE = texelFetchOffset(pressure, fragCoord, 0, ivec3(1, 0, 0)).x;
	float pN = texelFetchOffset(pressure, fragCoord, 0, ivec3(0, 1, 0)).x;
	float pU = texelFetchOffset(pressure, fragCoord, 0, ivec3(0, 0, 1)).x;
	float pW = texelFetchOffset(pressure, fragCoord, 0, ivec3(-1, 0, 0)).x;
	float pS = texelFetchOffset(pressure, fragCoord, 0, ivec3(0, -1, 0)).x;
	float pD = texelFetchOffset(pressure, fragCoord, 0, ivec3(0, 0, -1)).x;

	newVelocity = texelFetch(velocity, fragCoord, 0).xyz;
	newVelocity -= gradScale * vec3(pE - pW, pN - pS, pU - pD);
}