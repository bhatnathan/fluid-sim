#include "fluid.h"

#include <iostream>

Fluid::Fluid() {
	this->width = 0;
	this->height = 0;
	this->depth = 0;
	this->solverIterations = 0;
	this->dissipation = 0;
	this->fluidBouyancy = 0;
	this->fluidWeight = 0;
}

Fluid::Fluid(int width, int height, int depth, int solverIterations, float dissipation, float fluidBouyancy, float fluidWeight) {
	this->width = width;
	this->height = height;
	this->depth = depth;
	this->solverIterations = solverIterations;
	this->dissipation = dissipation;
	this->fluidBouyancy = fluidBouyancy;
	this->fluidWeight = fluidWeight;

	this->advectShader = Shader("shaders/base.vert", "shaders/base.geom", "shaders/advect.frag");
	this->divergenceShader = Shader("shaders/base.vert", "shaders/base.geom", "shaders/divergence.frag");
	this->gradsubShader = Shader("shaders/base.vert", "shaders/base.geom", "shaders/gradsub.frag");
	this->jacobiShader = Shader("shaders/base.vert", "shaders/base.geom", "shaders/jacobi.frag");
	this->boundaryShader = Shader("shaders/base.vert", "shaders/base.geom", "shaders/boundary.frag");
	this->splatShader = Shader("shaders/base.vert", "shaders/base.geom", "shaders/splat.frag");
	this->bouyancyShader = Shader("shaders/base.vert", "shaders/base.geom", "shaders/bouyancy.frag");
	this->drawShader = Shader("shaders/draw.vert", "shaders/draw.geom", "shaders/draw.frag");

	this->velocity = Buffer(width, height, depth, 3);
	this->density = Buffer(width, height, depth, 1);
	this->pressure = Buffer(width, height, depth, 1);
	this->temperature = Buffer(width, height, depth, 1);
	this->div = Buffer(width, height, depth, 3);
}

Fluid::~Fluid() {
}

void Fluid::update(float dt, GLuint quadVBO) {
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glVertexAttribPointer(0, 2, GL_SHORT, GL_FALSE, 2 * sizeof(short), 0);
	glViewport(0, 0, width, height);

	advect(velocity, dt);
	advect(density, dt);

	//Apply forces
	bouyancy(dt);
	splat(density, glm::vec3(0.5 * width, 0.5 * height, 0.5 * depth), 10, 1); //TODO add splats somewhere else
	splat(temperature, glm::vec3(0.5 * width, 0.5 * height, 0.5 * depth), 10, 1);

	divergence();
	pressure.clear();
	for (int i = 0; i < solverIterations; i++)
		jacobi();
	gradsub();

	boundary();
}

//TODO remove unused variables
void Fluid::render(GLuint boxVBO, glm::mat4 modelView, glm::mat4 view, glm::mat4 projection, glm::mat4 mvp, int screenWidth, int screenHeight) {
	drawShader.use();

	drawShader.setMatrix("mvp", mvp);
	drawShader.setMatrix("modelView", modelView);

	drawShader.setVec3("rayOrigin", glm::vec3(glm::transpose(modelView) * glm::vec4(0, 0, 3.5f, 1))); //TODO Extract eye height from view?
	drawShader.setFloat("focalLength", 1.0f / std::tan(glm::radians(45.0f) / 2));
	drawShader.setVec2("screenSize", glm::vec2(screenWidth, screenHeight));

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, screenWidth, screenHeight);
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	glEnable(GL_BLEND);

	// Draw ink:
	glBindBuffer(GL_ARRAY_BUFFER, boxVBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, density.in.colorTexture);

	glDrawArrays(GL_POINTS, 0, 1);
	glDisable(GL_BLEND);
}

void Fluid::resetState() {
	glActiveTexture(GL_TEXTURE2); glBindTexture(GL_TEXTURE_3D, 0);
	glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_3D, 0);
	glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_3D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_BLEND);
}

void Fluid::advect(Buffer& toAdvect, float dt) {
	advectShader.use();

	advectShader.setInt("velocity", 0);
	advectShader.setInt("advecting", 1);
	advectShader.setFloat("timeStep", dt);
	advectShader.setFloat("dissipation", dissipation);
	advectShader.setVec3("inverseBoxSize", glm::vec3(1.0 / width, 1.0 / height, 1.0 / depth));

	glBindFramebuffer(GL_FRAMEBUFFER, toAdvect.out.fboHandle);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, velocity.in.colorTexture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_3D, toAdvect.in.colorTexture);

	glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, toAdvect.out.depth);

	resetState();
	toAdvect.swapFrameBuffers();
}

void Fluid::divergence() {
	divergenceShader.use();

	divergenceShader.setInt("vecField", 0);
	divergenceShader.setFloat("divergenceMod", 0.5);

	glBindFramebuffer(GL_FRAMEBUFFER, div.out.fboHandle);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, velocity.in.colorTexture);

	glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, div.out.depth);

	resetState();
	div.swapFrameBuffers();
}

void Fluid::gradsub() {
	gradsubShader.use();

	gradsubShader.setInt("velocity", 0);
	gradsubShader.setInt("pressure", 1);
	gradsubShader.setVec2("screenSize", glm::vec2(width, height));
	gradsubShader.setFloat("gradScale", 0.5);

	glBindFramebuffer(GL_FRAMEBUFFER, velocity.out.fboHandle);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, velocity.in.colorTexture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_3D, pressure.in.colorTexture);

	glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, velocity.out.depth);

	resetState();
	velocity.swapFrameBuffers();
}

void Fluid::jacobi() {
	jacobiShader.use();

	jacobiShader.setInt("xVec", 0);
	jacobiShader.setInt("bVec", 1);
	jacobiShader.setFloat("alpha", -1.0);
	jacobiShader.setFloat("inverseBeta", 1.0 / 6.0);

	glBindFramebuffer(GL_FRAMEBUFFER, pressure.out.fboHandle);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, pressure.in.colorTexture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_3D, div.in.colorTexture);

	glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, pressure.out.depth);

	resetState();
	pressure.swapFrameBuffers();
}

void Fluid::boundary() {
	boundaryShader.use();

	boundaryShader.setInt("velocity", 0);
	boundaryShader.setVec3("boxSize", glm::vec3(width, height, depth));

	glBindFramebuffer(GL_FRAMEBUFFER, velocity.out.fboHandle);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, velocity.in.colorTexture);

	glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, velocity.out.depth);

	resetState();
	velocity.swapFrameBuffers();
}

void Fluid::splat(Buffer& toSplat, glm::vec3 positon, float radius, float value) {
	splatShader.use();

	splatShader.setVec3("point", positon);
	splatShader.setFloat("radius", radius);
	splatShader.setVec3("splatAmount", glm::vec3(value, value, value));

	glBindFramebuffer(GL_FRAMEBUFFER, toSplat.in.fboHandle);
	glEnable(GL_BLEND);

	glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, toSplat.in.depth);

	resetState();
}

void Fluid::bouyancy(float dt) {
	bouyancyShader.use();

	boundaryShader.setInt("velocity", 0);
	bouyancyShader.setInt("temperature", 1);
	bouyancyShader.setInt("density", 2);
	bouyancyShader.setFloat("timeStep", dt);
	bouyancyShader.setFloat("bouyancy", fluidBouyancy);
	bouyancyShader.setFloat("weight", fluidWeight);

	glBindFramebuffer(GL_FRAMEBUFFER, velocity.out.fboHandle);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, velocity.in.colorTexture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_3D, temperature.in.colorTexture);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_3D, density.in.colorTexture);

	glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, velocity.out.depth);

	resetState();
	velocity.swapFrameBuffers();
}