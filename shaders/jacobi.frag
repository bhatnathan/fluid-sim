#version 330 core

// Output data
out vec4 jacobiOut;

uniform sampler3D xVec;
uniform sampler3D bVec; //Ax = b

uniform float alpha;
uniform float inverseBeta;

in float gLayer;

void main() {
	ivec3 fragCoord = ivec3(gl_FragCoord.xy, gLayer);

	vec4 xE = texelFetchOffset(xVec, fragCoord, 0, ivec3(1, 0, 0));
	vec4 xN = texelFetchOffset(xVec, fragCoord, 0, ivec3(0, 1, 0));
	vec4 xU = texelFetchOffset(xVec, fragCoord, 0, ivec3(0, 0, 1));
	vec4 xW = texelFetchOffset(xVec, fragCoord, 0, ivec3(-1, 0, 0));
	vec4 xS = texelFetchOffset(xVec, fragCoord, 0, ivec3(0, -1, 0));
	vec4 xD = texelFetchOffset(xVec, fragCoord, 0, ivec3(0, 0, -1));

	// b sample, from center
	vec4 bC = texelFetch(bVec, fragCoord, 0);

	// evaluate Jacobi iteration
	jacobiOut = (xE + xN + xU + xW + xS + xD + alpha * bC) * inverseBeta;
}