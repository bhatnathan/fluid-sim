#version 330 core

// Output Data
out vec4 FragColor;

uniform sampler2D velocity;
uniform sampler2D advected;

uniform vec2 inverseSize;
uniform float timestep;
uniform float dissipation;

void main()
{
    vec2 fragCoord = gl_FragCoord.xy;

    vec2 u = texture(velocity, inverseSize * fragCoord).xy;
    vec2 coord = inverseSize * (fragCoord - timestep * u);
    FragColor = dissipation * texture(advected, coord);
}