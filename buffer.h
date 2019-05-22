#pragma once

#include "include\glew\glew.h"

struct FrameBuffer {
	GLuint fboHandle;
	GLuint colorTexture;
	GLsizei width;
	GLsizei height;
	GLsizei depth;

	FrameBuffer();
	FrameBuffer(GLsizei width, GLsizei height, GLsizei depth, unsigned int numComponents);
};

class Buffer {
public:
	Buffer();
	Buffer(GLsizei width, GLsizei height, GLsizei depth, unsigned int numComponents);
	~Buffer();

	void swapFrameBuffers();
	void clear();

	FrameBuffer in;
	FrameBuffer out;
};

