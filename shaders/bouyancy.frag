#version 330

// Output Data
out vec3 velocityOut;

// Input Data
in float gLayer;

uniform sampler3D velocity;
uniform sampler3D temperature;
uniform sampler3D density;

uniform float timeStep;
uniform float bouyancy;
uniform float weight;

void main() {
    ivec3 fragCoord = ivec3(gl_FragCoord.xy, gLayer);

	float temperatureValue = texelFetch(temperature, fragCoord, 0).r;
	float densityValue = texelFetch(density, fragCoord, 0).x;

    velocityOut = texelFetch(velocity, fragCoord, 0).xyz;
    velocityOut += (timeStep * (temperatureValue) * bouyancy - densityValue * weight) * vec3(0, 1, 0);
}