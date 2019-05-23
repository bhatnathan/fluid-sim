#pragma once

#include "shader.h"
#include "buffer.h"
#include "transform.h"

class Fluid {
public:
	Fluid();
	Fluid(int width, int height, int depth, int solverIterations, float dissipation, float fluidBouyancy, float fluidWeight);
	~Fluid();

	void update(float dt, GLuint quadVBO);
	void render(GLuint boxVBO, glm::mat4 view, glm::mat4 projection, int screenWidth, int screenHeight);

	void puff();
	void toggleAuto();

	Transform& getTransform();
private:
	int width;
	int height;
	int depth;
	int solverIterations;
	float dissipation;
	float fluidBouyancy;
	float fluidWeight;
	bool applyPuff;
	bool autoRun;

	Shader advectShader;
	Shader maccormackShader;
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
	Buffer phi;

	Transform transform;

	void resetState();

	void advect(Buffer& toAdvect, float dt);
	void advectHigher(Buffer& toAdvect, float dt);
	void advect(Buffer& toAdvect, float dt, Buffer& phi);
	void maccormack(Buffer& toAdvect, float dt, Buffer& phi);
	void divergence();
	void gradsub();
	void jacobi();
	void boundary();
	void splat(Buffer& toSplat, glm::vec3 positon, float radius, glm::vec3 value);
	void bouyancy(float dt);
};

