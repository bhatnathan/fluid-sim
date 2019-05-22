#version 330 core

// Output Data
out vec4 color;

uniform sampler2D toDraw;
uniform vec3 fillColor;
uniform vec2 inverseScreenSize;

void main()
{
	vec2 fragCoord = gl_FragCoord.xy;

    float l = texture(toDraw, fragCoord * inverseScreenSize).r;
    color = vec4(fillColor, l);
}