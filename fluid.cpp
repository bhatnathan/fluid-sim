#include "fluid.h"

#include <iostream>

Fluid::Fluid() {
	this->width = 0;
	this->height = 0;
	this->solverIterations = 0;
	this->dissipation = 0;
	this->fluidBouyancy = 0;
	this->fluidWeight = 0;
}

Fluid::Fluid(int width, int height, int solverIterations, float dissipation, float fluidBouyancy, float fluidWeight) {
	this->width = width;
	this->height = height;
	this->solverIterations = solverIterations;
	this->dissipation = dissipation;
	this->fluidBouyancy = fluidBouyancy;
	this->fluidWeight = fluidWeight;

	this->advectShader = Shader("shaders/base.vert", "shaders/advect.frag");
	this->divergenceShader = Shader("shaders/base.vert", "shaders/divergence.frag");
	this->gradsubShader = Shader("shaders/base.vert", "shaders/gradsub.frag");
	this->jacobiShader = Shader("shaders/base.vert", "shaders/jacobi.frag");
	this->boundaryShader = Shader("shaders/base.vert", "shaders/boundary.frag");
	this->splatShader = Shader("shaders/base.vert", "shaders/splat.frag");
	this->bouyancyShader = Shader("shaders/base.vert", "shaders/bouyancy.frag");
	this->drawShader = Shader("shaders/base.vert", "shaders/draw.frag");

	this->velocity = Buffer(width, height, 2);
	this->density = Buffer(width, height, 1);
	this->pressure = Buffer(width, height, 1);
	this->temperature = Buffer(width, height, 1);
	this->div = Buffer(width, height, 3);
}

Fluid::~Fluid() {
}

void Fluid::update(float dt) {
	glViewport(0, 0, width, height);

	advect(velocity, dt);
	advect(density, dt);

	//Apply forces
	bouyancy(dt);
	splat(density, glm::vec2(0.5 * width, 0.5 * height), 30, 1); //TODO add splats somewhere else
	splat(temperature, glm::vec2(0.5 * width, 0.5 * height), 30, 1);

	divergence();
	pressure.clear();
	for (int i = 0; i < solverIterations; i++)
		jacobi();
	gradsub();

	boundary();
}

void Fluid::render() {
	drawShader.use();

	drawShader.setInt("toDraw", 0);
	drawShader.setVec3("fillColor", glm::vec3(1, 1, 1)); //TODO fun color function?
	drawShader.setVec2("inverseScreenSize", glm::vec2(1.0 / width, 1.0 / height));

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Set render target to the backbuffer:
	glViewport(0, 0, width, height);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	// Draw ink:
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, density.in.textureHandle);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glDisable(GL_BLEND);
}

void Fluid::resetState() {
	glActiveTexture(GL_TEXTURE2); glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_BLEND);
}

void Fluid::advect(Buffer& toAdvect, float dt) {
	advectShader.use();

	advectShader.setInt("velocity", 0);
	advectShader.setInt("advecting", 1);
	advectShader.setFloat("timeStep", dt);
	advectShader.setFloat("dissipation", dissipation);
	advectShader.setVec2("inverseScreenSize", glm::vec2(1.0 / width, 1.0 / height));

	glBindFramebuffer(GL_FRAMEBUFFER, toAdvect.out.fboHandle);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, velocity.in.textureHandle);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, toAdvect.in.textureHandle);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	resetState();
	toAdvect.swapFrameBuffers();
}

void Fluid::divergence() {
	divergenceShader.use();

	divergenceShader.setInt("vecField", 0);
	divergenceShader.setFloat("divergenceMod", 0.5);

	glBindFramebuffer(GL_FRAMEBUFFER, div.out.fboHandle);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, velocity.in.textureHandle);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	resetState();
	div.swapFrameBuffers();
}

void Fluid::gradsub() {
	gradsubShader.use();

	gradsubShader.setInt("velocity", 0);
	gradsubShader.setInt("pressure", 1);
	gradsubShader.setVec2("screenSize", glm::vec2(width, height));

	glBindFramebuffer(GL_FRAMEBUFFER, velocity.out.fboHandle);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, velocity.in.textureHandle);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, pressure.in.textureHandle);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	resetState();
	velocity.swapFrameBuffers();
}

void Fluid::jacobi() {
	jacobiShader.use();

	jacobiShader.setInt("xVec", 0);
	jacobiShader.setInt("bVec", 1);
	jacobiShader.setFloat("alpha", -1.0);
	jacobiShader.setFloat("inverseBeta", 0.25);

	glBindFramebuffer(GL_FRAMEBUFFER, pressure.out.fboHandle);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, pressure.in.textureHandle);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, div.in.textureHandle);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	resetState();
	pressure.swapFrameBuffers();
}

void Fluid::boundary() {
	boundaryShader.use();

	boundaryShader.setInt("velocity", 0);
	boundaryShader.setVec2("screenSize", glm::vec2(width, height));

	glBindFramebuffer(GL_FRAMEBUFFER, velocity.out.fboHandle);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, velocity.in.textureHandle);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	resetState();
	velocity.swapFrameBuffers();
}

void Fluid::splat(Buffer& toSplat, glm::vec2 positon, float radius, float value) {
	splatShader.use();

	splatShader.setVec2("point", positon);
	splatShader.setFloat("radius", radius);
	splatShader.setVec3("splatAmount", glm::vec3(value, value, value));

	glBindFramebuffer(GL_FRAMEBUFFER, toSplat.in.fboHandle);
	glEnable(GL_BLEND);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

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
	glBindTexture(GL_TEXTURE_2D, velocity.in.textureHandle);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, temperature.in.textureHandle);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, density.in.textureHandle);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	resetState();
	velocity.swapFrameBuffers();
}