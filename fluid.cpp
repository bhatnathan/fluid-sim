#include "fluid.h"

#include <iostream>

Fluid::Fluid() {

}

Fluid::Fluid(int width, int height, int solverIterations) {
	this->width = width;
	this->height = height;
	this->solverIterations = solverIterations;

	this->advectShader = Shader("shaders/base.vert", "shaders/advect.frag");
	this->divergenceShader = Shader("shaders/base.vert", "shaders/divergence.frag");
	this->gradsubShader = Shader("shaders/base.vert", "shaders/gradsub.frag");
	this->jacobiShader = Shader("shaders/base.vert", "shaders/jacobi.frag");
	this->boundaryShader = Shader("shaders/base.vert", "shaders/boundary.frag");
	this->splatShader = Shader("shaders/base.vert", "shaders/splat.frag");
	this->gravityShader = Shader("shaders/base.vert", "shaders/gravity.frag");
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
	gravity(dt);
	splat(density, glm::vec2(0.5 * width, 0.5 * height), 30, 1);
	splat(temperature, glm::vec2(0.5 * width, 0.5 * height), 30, 1);

	divergence();
	pressure.clear();
	for (int i = 0; i < solverIterations; i++)
		jacobi();
	gradsub();
}

void Fluid::render() {
	drawShader.use();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //TODO possibly put in main in initProgram

	// Set render target to the backbuffer:
	glViewport(0, 0, width, height); //TODO might need to be width and height * 2
	glBindFramebuffer(GL_FRAMEBUFFER, 0); //TODO not sure if 0 is correct
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	// Draw ink:
	glBindTexture(GL_TEXTURE_2D, density.in.textureHandle);
	drawShader.setVec3("FillColor", glm::vec3(1, 1, 1));
	drawShader.setVec2("Scale", glm::vec2(1.0 / width, 1.0 / height));//TODO might need to be width and height * 2
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

	advectShader.setFloat("timestep", dt);
	advectShader.setVec2("inverseSize", glm::vec2(1.0 / width, 1.0 / height)); //TODO look up what to set this to.
	advectShader.setInt("advected", 1); //TODO not sure if correct
	advectShader.setFloat("dissipation", 1.0); //TODO move dissipation to some nice variable

	glBindFramebuffer(GL_FRAMEBUFFER, toAdvect.out.fboHandle);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, velocity.in.textureHandle);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, toAdvect.in.textureHandle);
	//glActiveTexture(GL_TEXTURE2); //TODO this was for the obstacles but I don't know if it is still needed
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	resetState();
	toAdvect.swapFrameBuffers();
}

void Fluid::divergence() {
	divergenceShader.use();

	divergenceShader.setFloat("divergenceMod", 0.5);

	glBindFramebuffer(GL_FRAMEBUFFER, div.out.fboHandle);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, velocity.in.textureHandle);
	//glActiveTexture(GL_TEXTURE1); //TODO this was for the obstacles but I don't know if it is still needed
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	resetState();
	div.swapFrameBuffers();
}

void Fluid::gradsub() {
	gradsubShader.use();

	gradsubShader.setInt("pressure", 1);
	gradsubShader.setVec2("screenSize", glm::vec2(width, height));

	glBindFramebuffer(GL_FRAMEBUFFER, velocity.out.fboHandle);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, velocity.in.textureHandle);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, pressure.in.textureHandle);
	//glActiveTexture(GL_TEXTURE2); //TODO this was for the obstacles but I don't know if it is still needed
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	resetState();
	velocity.swapFrameBuffers();
}

void Fluid::jacobi() {
	jacobiShader.use();

	jacobiShader.setFloat("alpha", -1.0); //TODO No idea if this is right
	jacobiShader.setFloat("rBeta", 0.25);
	jacobiShader.setInt("bVec", 1);

	glBindFramebuffer(GL_FRAMEBUFFER, pressure.out.fboHandle);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, pressure.in.textureHandle);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, div.in.textureHandle);
	//glActiveTexture(GL_TEXTURE2); //TODO this was for the obstacles but I don't know if it is still needed
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	resetState();
	pressure.swapFrameBuffers();
}

void Fluid::splat(Buffer& toSplat, glm::vec2 positon, float radius, float value) {
	splatShader.use();

	splatShader.setVec2("Point", positon);
	splatShader.setFloat("Radius", radius);
	splatShader.setVec3("FillColor", glm::vec3(value, value, value));

	glBindFramebuffer(GL_FRAMEBUFFER, toSplat.in.fboHandle);
	glEnable(GL_BLEND);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	resetState();
}

void Fluid::gravity(float dt) {
	gravityShader.use();

	gravityShader.setFloat("timeStep", dt);
	gravityShader.setInt("temperature", 1);
	gravityShader.setInt("density", 2);

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