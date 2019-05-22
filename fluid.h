#pragma once

#include "shader.h"
#include "buffer.h"

class Fluid {
public:
	Fluid();
	Fluid(int width, int height, int depth, int solverIterations, float dissipation, float fluidBouyancy, float fluidWeight);
	~Fluid();

	void update(float dt, GLuint quadVBO);
	void render(GLuint boxVBO, glm::mat4 model, glm::mat4 view, glm::mat4 projection, glm::mat4 mvp, int screenWidth, int screenHeight);
private:
	int width;
	int height;
	int depth;
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
	void splat(Buffer& toSplat, glm::vec3 positon, float radius, float value);
	void bouyancy(float dt);
};

