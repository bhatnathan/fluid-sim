#version 330 core

//From tutorial

// Output Data
out vec4 splatOut;

// Input Data
in float gLayer;

uniform vec3 point;
uniform float radius;
uniform vec3 splatAmount;

void main() {
	vec3 fragCoord = vec3(gl_FragCoord.xy, gLayer);

    float d = distance(point, fragCoord); 
    if (d < radius) { //splat in radius
        float a = (radius - d) * 0.5;
        a = min(a, 1.0);
        splatOut = vec4(splatAmount, a);
    } else {
        splatOut = vec4(0);
    }
}