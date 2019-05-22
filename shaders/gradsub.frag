#version 330 core

// Output data
out vec2 newVelocity;

uniform sampler2D pressure;
uniform sampler2D velocity;
uniform vec2 screenSize;

void main() {
	ivec2 coords = ivec2(gl_FragCoord.xy);

	float pL = texelFetchOffset(pressure, coords, 0, ivec2(-1, 0)).x;
	float pR = texelFetchOffset(pressure, coords, 0, ivec2(1, 0)).x;
	float pB = texelFetchOffset(pressure, coords, 0, ivec2(0, -1)).x;
	float pT = texelFetchOffset(pressure, coords, 0, ivec2(0, 1)).x; //not sure if .x is correct, hopefully though

	newVelocity = texelFetch(velocity, coords, 0).xy;
	newVelocity.xy -= vec2(pR - pL, pT - pB);
}