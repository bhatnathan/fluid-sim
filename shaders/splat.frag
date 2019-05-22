#version 330 core

// Output Data
out vec4 splatOut;

uniform vec2 point;
uniform float radius;
uniform vec3 splatAmount;

void main()
{
	vec2 fragCoord = gl_FragCoord.xy;

    float d = distance(point, fragCoord);
    if (d < radius) {
        float a = (radius - d) * 0.5;
        a = min(a, 1.0);
        splatOut = vec4(splatAmount, a);
    } else {
        splatOut = vec4(0);
    }
}