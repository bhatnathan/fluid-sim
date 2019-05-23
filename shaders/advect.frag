#version 330 core

// Output Data
out vec4 advectOut;

uniform sampler2D velocity;
uniform sampler2D advecting;

uniform float timeStep;
uniform float dissipation;
uniform vec2 inverseScreenSize;

vec2 bilerp(sampler2D d, vec2 p)
{
    vec2 uv = (floor(p - 0.5) + 0.5) * inverseScreenSize.xy;
    vec2 d11 = texture2D(d, uv + vec2(0,0) * inverseScreenSize.xy).xy;
    vec2 d21 = texture2D(d, uv + vec2(1,0) * inverseScreenSize.xy).xy;
    vec2 d12 = texture2D(d, uv + vec2(0,1) * inverseScreenSize.xy).xy;
    vec2 d22 = texture2D(d, uv + vec2(1,1) * inverseScreenSize.xy).xy;

    vec2 a = p - (floor(p - 0.5) + 0.5);

    return mix(mix(d11, d21, a.x), mix(d12, d22, a.x), a.y);
}

void main()
{
    vec2 fragCoord = vec2(gl_FragCoord.xy);
	
	vec2 u = fragCoord * inverseScreenSize.xy;

    // trace point back in time
    vec2 p = fragCoord - timeStep * texture2D(velocity, u).xy;

    advectOut = vec4(dissipation * bilerp(advecting, p), 0.0, 1.0);

}