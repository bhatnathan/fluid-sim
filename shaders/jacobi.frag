#version 330 core

// Output data
out vec4 targetOut;

uniform sampler2D xVec;
uniform sampler2D bVec; //Ax = b

uniform float alpha;
uniform float rBeta;

void main() {
	ivec2 coords = ivec2(gl_FragCoord.xy);

	vec4 xL = texelFetchOffset(xVec, coords, 0, ivec2(-1, 0));
	vec4 xR = texelFetchOffset(xVec, coords, 0, ivec2(1, 0));
	vec4 xB = texelFetchOffset(xVec, coords, 0, ivec2(0, -1));
	vec4 xT = texelFetchOffset(xVec, coords, 0, ivec2(0, 1));

	// b sample, from center
  
	vec4 bC = texelFetch(bVec, coords, 0);

	// evaluate Jacobi iteration
	targetOut = (xL + xR + xB + xT + alpha * bC) * rBeta;
}