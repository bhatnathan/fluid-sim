#version 330 core

//From tutorial

// Output Data
out vec4 fragColor;

uniform sampler3D densitySampler;

uniform vec3 lightPosition = vec3(0.0, 0.0, 3.5);
uniform vec3 lightIntensity = vec3(10.0);
uniform float absorption = 10.0;
uniform mat4 modelView;
uniform float focalLength;
uniform vec2 screenSize;
uniform vec3 rayOrigin;

const float maxDist = sqrt(2.0);
const int numSamples = 256; //increase for higher res, costly though
const float stepSize = maxDist/float(numSamples);
const int numLightSamples = 32;
const float lscale = maxDist / float(numLightSamples);
const float densityFactor = 10;

float getDensity(vec3 pos)
{
    return texture(densitySampler, pos).x * densityFactor;
}

struct Ray {
    vec3 origin;
    vec3 dir;
};

struct AABB {
    vec3 minV;
    vec3 maxV;
};

bool IntersectBox(Ray r, AABB aabb, out float t0, out float t1)
{
    vec3 invR = 1.0 / r.dir;
    vec3 tbot = invR * (aabb.minV-r.origin);
    vec3 ttop = invR * (aabb.maxV-r.origin);
    vec3 tmin = min(ttop, tbot);
    vec3 tmax = max(ttop, tbot);
    vec2 t = max(tmin.xx, tmin.yz);
    t0 = max(t.x, t.y);
    t = min(tmax.xx, tmax.yz);
    t1 = min(t.x, t.y);
    return t0 <= t1;
}

void main()
{
    vec3 rayDirection;
    rayDirection.xy = 2.0 * gl_FragCoord.xy / screenSize - 1.0;
    rayDirection.z = -focalLength;
    rayDirection = (vec4(rayDirection, 0) * modelView).xyz;

    Ray eye = Ray( rayOrigin, normalize(rayDirection) );
    AABB aabb = AABB(vec3(-1.0), vec3(+1.0));

    float tnear, tfar;
    IntersectBox(eye, aabb, tnear, tfar);
    if (tnear < 0.0) tnear = 0.0;

    vec3 rayStart = eye.origin + eye.dir * tnear;
    vec3 rayStop = eye.origin + eye.dir * tfar;
    rayStart = 0.5 * (rayStart + 1.0);
    rayStop = 0.5 * (rayStop + 1.0);

    vec3 pos = rayStart;
    vec3 step = normalize(rayStop-rayStart) * stepSize;
    float travel = distance(rayStop, rayStart);
    float t = 1.0;
    vec3 lo = vec3(0.0);

    for (int i=0; i < numSamples && travel > 0.0; ++i, pos += step, travel -= stepSize) {

        float density = getDensity(pos);
        if (density <= 0.0)
            continue;

        t *= 1.0-density*stepSize*absorption;
        if (t <= 0.01)
            break;

        vec3 lightDir = normalize(lightPosition-pos)*lscale;
        float tl = 1.0;
        vec3 lpos = pos + lightDir;

        for (int s=0; s < numLightSamples; ++s) {
            float ld = texture(densitySampler, lpos).x;
            tl *= 1.0-absorption*stepSize*ld;
            if (tl <= 0.01) 
            lpos += lightDir;
        }

        vec3 li = lightIntensity*tl;
        lo += li*t*density*stepSize;
    }

    fragColor.rgb = lo;
    fragColor.a = 1-t;
}
