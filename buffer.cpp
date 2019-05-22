#include "buffer.h"

#include <iostream>

FrameBuffer::FrameBuffer() {
	this->fboHandle = 0;
	this->colorTexture = 0;
	this->width = 1;
	this->height = 1;
	this->depth = 1;
}

FrameBuffer::FrameBuffer(GLsizei width, GLsizei height, GLsizei depth, unsigned int numComponents) {
	this->fboHandle = 0;
	this->colorTexture = 0;
	this->width = width;
	this->height = height;
	this->depth = depth;

	glGenFramebuffers(1, &fboHandle);
	glBindFramebuffer(GL_FRAMEBUFFER, fboHandle);

	glGenTextures(1, &colorTexture);
	glBindTexture(GL_TEXTURE_3D, colorTexture);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	switch (numComponents) {
		case 1: 
			glTexImage3D(GL_TEXTURE_3D, 0, GL_R16F, width, height, depth, 0, GL_RED, GL_HALF_FLOAT, 0);
			break;
		case 2: 
			glTexImage3D(GL_TEXTURE_3D, 0, GL_RG16F, width, height, depth, 0, GL_RG, GL_HALF_FLOAT, 0);
			break;
		case 3: 
			glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB16F, width, height, depth, 0, GL_RGB, GL_HALF_FLOAT, 0);
			break;
		case 4: 
			glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA16F, width, height, depth, 0, GL_RGBA, GL_HALF_FLOAT, 0); 
			break;
		default: 
			throw "Illegal number of FrameBuffer channels.";
	}
	

	if (GL_NO_ERROR != glGetError())
		throw "Unable to create normals texture";

	GLuint colorbuffer;
	glGenRenderbuffers(1, &colorbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, colorbuffer);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, colorTexture, 0);
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

Buffer::Buffer(GLsizei width, GLsizei height, GLsizei depth, unsigned int numComponents) {
	this->in = FrameBuffer(width, height, depth, numComponents);
	this->out = FrameBuffer(width, height, depth, numComponents);
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