#version 330 core

// Output Data
out vec4 advectOut;

uniform sampler3D velocity;
uniform sampler3D advecting;

uniform float timeStep;
uniform float dissipation;
uniform vec3 inverseScreenSize;

void main()
{
    vec3 fragCoord = gl_FragCoord.xyz; //TODO not sure if correct, guid uses an in gLayer as z component

    vec3 u = texture(velocity, inverseScreenSize * fragCoord).xyz;
    vec3 coord = inverseScreenSize * (fragCoord - timeStep * u);
    advectOut = dissipation * texture(advecting, coord);

}