#version 330 core

//Input Data
in vec2 coords;

// Output data
out vec4 targetOut;

uniform float alpha;
uniform float rBeta;
uniform sampler2D xVec;
uniform sampler2D bVec; //Ax = b

void main() {
	vec4 xL = texture2D(xVec, coords - vec2(1, 0)).xyzw;
	vec4 xR = texture2D(xVec, coords + vec2(1, 0)).xyzw;
	vec4 xB = texture2D(xVec, coords - vec2(0, 1)).xyzw;
	vec4 xT = texture2D(xVec, coords + vec2(0, 1)).xyzw;
	// b sample, from center
  
	vec4 bC = texture2D(bVec, coords).xyzw;
	// evaluate Jacobi iteration
	targetOut = (xL + xR + xB + xT + alpha * bC) * rBeta;
}