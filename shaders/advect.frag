#version 330 core

// Output Data
out vec4 advectOut;

uniform sampler3D velocity;
uniform sampler3D advecting;

uniform float timeStep;
uniform float dissipation;
uniform vec3 inverseBoxSize;

in float gLayer;

vec3 trilerp(sampler3D d, vec3 p) {
    vec3 uv = (floor(p - 0.5) + 0.5) * inverseBoxSize;
    vec3 d000 = texture(d, uv + vec3(0,0,0) * inverseBoxSize).xyz;
    vec3 d100 = texture(d, uv + vec3(1,0,0) * inverseBoxSize).xyz;
    vec3 d010 = texture(d, uv + vec3(0,1,0) * inverseBoxSize).xyz;
    vec3 d110 = texture(d, uv + vec3(1,1,0) * inverseBoxSize).xyz;
	vec3 d001 = texture(d, uv + vec3(0,0,1) * inverseBoxSize).xyz;
    vec3 d101 = texture(d, uv + vec3(1,0,1) * inverseBoxSize).xyz;
    vec3 d011 = texture(d, uv + vec3(0,1,1) * inverseBoxSize).xyz;
    vec3 d111 = texture(d, uv + vec3(1,1,1) * inverseBoxSize).xyz;

    vec3 a = p - (floor(p - 0.5) + 0.5);

    return mix(mix(mix(d000, d100, a.x), mix(d010, d110, a.x), a.y), mix(mix(d001, d101, a.x), mix(d011, d111, a.x), a.y), a.z);
}

void main() {
    vec3 fragCoord = vec3(gl_FragCoord.xy, gLayer);

    vec3 u = texture(velocity, inverseBoxSize * fragCoord).xyz;
    vec3 coord = fragCoord - timeStep * u;
    advectOut = vec4(dissipation * trilerp(advecting, coord), 1.0);

}