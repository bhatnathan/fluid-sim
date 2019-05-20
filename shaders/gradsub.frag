#version 330 core

//Input Data
in vec2 coords;

// Output data
out vec4 newVelocity;

uniform float rdx;
uniform sampler2D pressure;
uniform sampler2D velocity;

void main() {
	float pL = texture2D(pressure, coords - vec2(1, 0)).x;
	float pR = texture2D(pressure, coords + vec2(1, 0)).x;
	float pB = texture2D(pressure, coords - vec2(0, 1)).x;
	float pT = texture2D(pressure, coords + vec2(0, 1)).x; //not sure if .x is correct, hopefully though
	newVelocity = texture2D(velocity, coords);
	newVelocity.xy -= rdx * vec2(pR - pL, pT - pB);
}