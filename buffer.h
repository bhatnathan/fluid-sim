#pragma once

#include "include\glew\glew.h"

struct FrameBuffer {
	GLuint fboHandle;
	GLuint textureHandle;
	int numComponents;

	FrameBuffer();
	FrameBuffer(GLsizei width, GLsizei height, unsigned int numComponents);
};

class Buffer {
public:
	Buffer();
	Buffer(GLsizei width, GLsizei height, unsigned int numComponents);
	~Buffer();

	void swapFrameBuffers();
	void clear();

	FrameBuffer in;
	FrameBuffer out;
};

