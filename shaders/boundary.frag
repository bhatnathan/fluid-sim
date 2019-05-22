#version 330 core

// Output data
out vec4 velocityOut;

uniform sampler2D velocity;
uniform vec2 screenSize;

void main() {

	ivec2 fragCoord = ivec2(gl_FragCoord.xy);

	velocityOut = texelFetch(velocity, fragCoord, 0);

	if (fragCoord.x - 1 < 0 || fragCoord.x + 2 > screenSize.x) {
        velocityOut.x = -texelFetch(velocity, fragCoord, 0).x;
    }

	if (fragCoord.y - 1 <  0 || fragCoord.y + 2 > screenSize.y) {
		velocityOut.y = -texelFetch(velocity, fragCoord, 0).y;
	}


}