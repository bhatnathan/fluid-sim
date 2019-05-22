#version 330

// Output Data
out vec2 velocityOut;

uniform sampler2D velocity;
uniform sampler2D temperature;
uniform sampler2D density;

uniform float timeStep;
uniform float bouyancy;
uniform float weight;

void main()
{
    ivec2 fragCoord = ivec2(gl_FragCoord.xy);

	float temperatureValue = texelFetch(temperature, fragCoord, 0).r;
	float densityValue = texelFetch(density, fragCoord, 0).x;

    velocityOut = texelFetch(velocity, fragCoord, 0).xy;
    velocityOut += (timeStep * (temperatureValue) * bouyancy - densityValue * weight) * vec2(0, 1);
}