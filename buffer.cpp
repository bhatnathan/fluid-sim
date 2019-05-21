#include "buffer.h"

#include <iostream>

FrameBuffer::FrameBuffer() {
	this->fboHandle = 0;
	this->textureHandle = 0;
	this->numComponents = 1;
}

FrameBuffer::FrameBuffer(GLsizei width, GLsizei height, unsigned int numComponents) {
	this->fboHandle = 0;
	this->textureHandle = 0;
	this->numComponents = numComponents;

	glGenFramebuffers(1, &fboHandle);
	glBindFramebuffer(GL_FRAMEBUFFER, fboHandle);

	glGenTextures(1, &textureHandle);
	glBindTexture(GL_TEXTURE_2D, textureHandle);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	switch (numComponents) {
		case 1: 
			glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width, height, 0, GL_RED, GL_FLOAT, 0); 
			break;
		case 2: 
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, width, height, 0, GL_RG, GL_FLOAT, 0); 
			break;
		case 3: 
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, 0); 
			break;
		case 4: 
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, 0); 
			break;
		default: 
			throw "Illegal number of FrameBuffer channels.";
	}
	

	if (GL_NO_ERROR != glGetError())
		throw "Unable to create normals texture";

	GLuint colorbuffer;
	glGenRenderbuffers(1, &colorbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, colorbuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureHandle, 0);
	if (GL_NO_ERROR != glGetError())
		throw "Unable to attach color buffer";

	if (GL_FRAMEBUFFER_COMPLETE != glCheckFramebufferStatus(GL_FRAMEBUFFER))
		throw"Unable to create FBO.";

	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Buffer::Buffer() {

}

Buffer::Buffer(GLsizei width, GLsizei height, unsigned int numComponents) {
	this->in = FrameBuffer(width, height, numComponents);
	this->out = FrameBuffer(width, height, numComponents);
}

Buffer::~Buffer() {
}

void Buffer::swapFrameBuffers() {
	FrameBuffer temp = this->in;
	this->in = this->out;
	this->out = temp;
}

void Buffer::clear() {
	glBindFramebuffer(GL_FRAMEBUFFER, in.fboHandle);
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);
}