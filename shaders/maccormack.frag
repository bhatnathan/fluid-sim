#version 330 core

// Output Data
out vec4 advectOut;

// Input Data
in float gLayer;

uniform sampler3D velocity;
uniform sampler3D phiHat1;
uniform sampler3D phiHat;
uniform sampler3D phi;

uniform float timeStep;
uniform float dissipation;
uniform vec3 inverseBoxSize;

vec3 phiMin;
vec3 phiMax;

void calcPhiMinMax(vec3 p) {
	vec3 uv = (floor(p - 0.5) + 0.5) * inverseBoxSize;
    vec3 d000 = texture(phi, uv + vec3(0,0,0) * inverseBoxSize).xyz;
    vec3 d100 = texture(phi, uv + vec3(1,0,0) * inverseBoxSize).xyz;
    vec3 d010 = texture(phi, uv + vec3(0,1,0) * inverseBoxSize).xyz;
    vec3 d110 = texture(phi, uv + vec3(1,1,0) * inverseBoxSize).xyz;
	vec3 d001 = texture(phi, uv + vec3(0,0,1) * inverseBoxSize).xyz;
    vec3 d101 = texture(phi, uv + vec3(1,0,1) * inverseBoxSize).xyz;
    vec3 d011 = texture(phi, uv + vec3(0,1,1) * inverseBoxSize).xyz;
    vec3 d111 = texture(phi, uv + vec3(1,1,1) * inverseBoxSize).xyz;

	phiMin = min(min(min(min(min(min(min(d000, d001), d010), d011), d100), d101), d110), d111);
	phiMax = max(max(max(max(max(max(max(d000, d001), d010), d011), d100), d101), d110), d111);

}

void main() {
    vec3 fragCoord = vec3(gl_FragCoord.xy, gLayer);

    vec3 u = texture(velocity, inverseBoxSize * fragCoord).xyz;
    vec3 coord = fragCoord - timeStep * u;
	calcPhiMinMax(coord);
	
	vec3 phiHatSample = texture(phiHat, inverseBoxSize * fragCoord).xyz;
	vec3 phiSample = texture(phi, inverseBoxSize * fragCoord).xyz;
	vec3 phiHat1Sample = texture(phiHat1, inverseBoxSize * fragCoord).xyz;

	vec3 result = phiHat1Sample + 0.5 * (phiSample - phiHatSample); //MacCormack step
	result = max(min(result, phiMax), phiMin); //Clamp so it doesn't diverge
    
	advectOut = vec4(dissipation * result, 1.0);
}