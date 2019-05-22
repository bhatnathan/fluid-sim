#pragma once

#include "shader.h"
#include "buffer.h"

class Fluid {
public:
	Fluid();
	Fluid(int width, int height, int solverIterations, float dissipation, float fluidBouyancy, float fluidWeight);
	~Fluid();

	void update(float dt);
	void render();
private:
	int width;
	int height;
	int solverIterations;
	float dissipation;
	float fluidBouyancy;
	float fluidWeight;

	Shader advectShader;
	Shader divergenceShader;
	Shader gradsubShader;
	Shader jacobiShader;
	Shader boundaryShader;
	Shader splatShader;
	Shader bouyancyShader;
	Shader drawShader;

	Buffer velocity;
	Buffer density;
	Buffer pressure;
	Buffer temperature;
	Buffer div;

	void resetState();

	void advect(Buffer& toAdvect, float dt);
	void divergence();
	void gradsub();
	void jacobi();
	void boundary();
	void splat(Buffer& toSplat, glm::vec2 positon, float radius, float value);
	void bouyancy(float dt);
};

