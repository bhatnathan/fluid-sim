#pragma once

#include "include\glew\glew.h"

//Holds info for a 3D Frame Buffer Object
struct FrameBuffer {
	GLuint fboHandle;
	GLuint colorTexture;
	GLsizei width;
	GLsizei height;
	GLsizei depth;

	FrameBuffer();
	FrameBuffer(GLsizei width, GLsizei height, GLsizei depth, unsigned int numComponents);
};

//A class to hold frame buffers so they can have both an in and out frame buffer and swap between them.
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

