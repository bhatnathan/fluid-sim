#version 330 core

// Output data
out vec3 newVelocity;

uniform sampler3D velocity;
uniform sampler3D pressure;

uniform vec2 screenSize;

void main() {
	ivec3 fragCoord = ivec3(gl_FragCoord.xyz);

	float pE = texelFetchOffset(pressure, fragCoord, 0, ivec3(1, 0, 0)).x;
	float pN = texelFetchOffset(pressure, fragCoord, 0, ivec3(0, 1, 0)).x;
	float pU = texelFetchOffset(pressure, fragCoord, 0, ivec3(0, 0, 1)).x;
	float pW = texelFetchOffset(pressure, fragCoord, 0, ivec3(-1, 0, 0)).x;
	float pS = texelFetchOffset(pressure, fragCoord, 0, ivec3(0, -1, 0)).x;
	float pD = texelFetchOffset(pressure, fragCoord, 0, ivec3(0, 0, -1)).x;

	newVelocity = texelFetch(velocity, fragCoord, 0).xyz;
	newVelocity -= 0.5 * vec3(pE - pW, pN - pS, pU - pD); //TODO make the 0.5 a parameter
}