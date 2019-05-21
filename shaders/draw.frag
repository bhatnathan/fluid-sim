#version 330 core

// Output Data
out vec4 FragColor;

uniform sampler2D Sampler;
uniform vec3 FillColor;
uniform vec2 Scale;

void main()
{
	vec2 coords = gl_FragCoord.xy;

    float L = texture(Sampler, coords * Scale).r;
    FragColor = vec4(FillColor, L);
}