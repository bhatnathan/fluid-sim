#version 330 core

// Output data
out vec2 newVelocity;

uniform sampler2D velocity;
uniform sampler2D pressure;

uniform vec2 screenSize;

void main() {
	ivec2 fragCoord = ivec2(gl_FragCoord.xy);

	float pR = texelFetchOffset(pressure, fragCoord, 0, ivec2(1, 0)).x;
	float pT = texelFetchOffset(pressure, fragCoord, 0, ivec2(0, 1)).x;
	float pL = texelFetchOffset(pressure, fragCoord, 0, ivec2(-1, 0)).x;
	float pB = texelFetchOffset(pressure, fragCoord, 0, ivec2(0, -1)).x;

	newVelocity = texelFetch(velocity, fragCoord, 0).xy;
	newVelocity.xy -= vec2(pR - pL, pT - pB);
}