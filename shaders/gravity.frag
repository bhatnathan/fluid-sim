#version 330

// Output Data
out vec2 velocityOut;

uniform sampler2D velocity;
uniform sampler2D temperature;
uniform sampler2D density;
uniform float timeStep;

void main()
{
    ivec2 coords = ivec2(gl_FragCoord.xy);

	float T = texelFetch(temperature, coords, 0).r;
    vec2 V = texelFetch(velocity, coords, 0).xy;

    velocityOut = V;

    float D = texelFetch(density, coords, 0).x;
    velocityOut += (timeStep * (T) * 300 - D ) * vec2(0, 1);
}