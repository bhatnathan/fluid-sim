#version 330 core

// Output Data
out vec4 velocityOut;

// Input Data
in float gLayer;

uniform sampler3D velocity;

uniform vec3 boxSize;

void main() {

	ivec3 fragCoord = ivec3(gl_FragCoord.xy, gLayer);

	velocityOut = texelFetch(velocity, fragCoord, 0);

	//Check if on box edges
	if (fragCoord.x - 1 < 0 || fragCoord.x + 2 > boxSize.x) {
        velocityOut.x = -texelFetch(velocity, fragCoord, 0).x;
    }

	if (fragCoord.y - 1 <  0 || fragCoord.y + 2 > boxSize.y) {
		velocityOut.y = -texelFetch(velocity, fragCoord, 0).y;
	}

	if (fragCoord.z - 1 <  0 || fragCoord.z + 2 > boxSize.z) {
		velocityOut.z = -texelFetch(velocity, fragCoord, 0).z;
	}


}