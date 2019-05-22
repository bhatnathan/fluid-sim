#version 330 core

// Output Data
out vec4 advectOut;

uniform sampler2D velocity;
uniform sampler2D advecting;

uniform vec2 inverseScreenSize;
uniform float timeStep;
uniform float dissipation;

void main()
{
    vec2 fragCoord = gl_FragCoord.xy;

    vec2 u = texture(velocity, inverseScreenSize * fragCoord).xy;
    vec2 coord = inverseScreenSize * (fragCoord - timeStep * u);
    advectOut = dissipation * texture(advecting, coord);

}