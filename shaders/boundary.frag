#version 330 core

// Output data
out vec4 velocityOut;

uniform sampler2D velocity;
uniform vec2 screenSize;

void main() {
	vec2 coords = gl_FragCoord.xy;
	ivec2 iCoords = ivec2(gl_FragCoord.xy);

	if (iCoords.x - 10 < 0 || iCoords.x + 10 > screenSize.x || iCoords.y - 10 <  0 || iCoords.y + 10 > screenSize.y) {
        velocityOut = -texelFetch(velocity, iCoords, 0);
        return;
    }

	velocityOut = texelFetch(velocity, iCoords, 0);


}