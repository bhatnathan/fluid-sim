#version 330 core

// Output data
out vec4 jacobiOut;

uniform sampler2D xVec;
uniform sampler2D bVec; //Ax = b

uniform float alpha;
uniform float inverseBeta;

void main() {
	ivec2 fragCoord = ivec2(gl_FragCoord.xy);

	vec4 xR = texelFetchOffset(xVec, fragCoord, 0, ivec2(1, 0));
	vec4 xT = texelFetchOffset(xVec, fragCoord, 0, ivec2(0, 1));
	vec4 xL = texelFetchOffset(xVec, fragCoord, 0, ivec2(-1, 0));
	vec4 xB = texelFetchOffset(xVec, fragCoord, 0, ivec2(0, -1));

	// b sample, from center
	vec4 bC = texelFetch(bVec, fragCoord, 0);

	// evaluate Jacobi iteration
	jacobiOut = (xR + xT + xL + xB + alpha * bC) * inverseBeta;
}