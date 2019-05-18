#pragma once

#include "transform.h"
#include "include\glm\glm.hpp"

#define IX(x, y, z) ((x) + (y) * size + (z) * size * size)
#define IV(vec) (IX(vec.x, vec.y, vec.z))

constexpr int iter = 4;
constexpr float fadeAmt = 0.9;

using namespace std;

class FluidBox {

public:
	FluidBox();
	FluidBox(int size, float diffusion, float viscosity);
	~FluidBox();

	void setTransform(Transform t);
	void render();

	void addDensity(ivec3 cell, float amount);
	void addVelocity(ivec3 cell, vec3 amount);

	void step(float dt);

private:
	void set_bnd(int b, float* x);
	void lin_solve(int b, float* x, float* x0, float a, float c);
	void diffuse(int b, float* x, float* x0, float diff, float dt);
	void project(float* velocX, float* velocY, float* velocZ, float* p, float* div);
	void advect(int b, float* d, float* d0, float* velocX, float* velocY, float* velocZ, float dt);

	void fade();

	const int size;
	const float diffusion;
	const float viscosity;

	float* s;
	float* density;

	float* Vx;
	float* Vy;
	float* Vz;

	float* Vx0;
	float* Vy0;
	float* Vz0;

	Transform transform;
};

