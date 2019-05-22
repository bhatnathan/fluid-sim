#version 330 core

// Output data
out vec4 velocityOut;

uniform sampler2D velocity;
uniform vec2 screenSize;

void main() {
	vec2 coords = gl_FragCoord.xy;
	ivec2 iCoords = ivec2(gl_FragCoord.xy);

	velocityOut = texelFetch(velocity, iCoords, 0);

	if (iCoords.x - 1 < 0 || iCoords.x + 2 > screenSize.x) {
        velocityOut.x = -texelFetch(velocity, iCoords, 0).x;
    }

	if (iCoords.y - 1 <  0 || iCoords.y + 2 > screenSize.y) {
		velocityOut.y = -texelFetch(velocity, iCoords, 0).y;
	}


}