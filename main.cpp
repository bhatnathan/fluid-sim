#include "include\glew\glew.h"
#include "include\freeglut\freeglut.h"
#include "include\glm\glm.hpp"
#include <iostream>

#include <chrono>

#include "fluidbox.h"
#include "transform.h"
#include "include\glm\gtc\type_ptr.hpp"

void initDraw(void);
void reshape(int width, int height);
void renderScene(void);
void idle(void);
void key(unsigned char k, int x, int y);

std::chrono::high_resolution_clock::time_point lastTime;

float zRot = 0;

FluidBox fb(25, 25, 5, 0, 0.00001);

int main(int argc, char** argv) {

	//TODO make this nicer and put in correct function
	vec3 pos = vec3(0, 0, -100);
	quat rot = angleAxis(radians(0.0f), vec3(0.0f, 0.0f, 1.0f));
	vec3 scale = vec3(1.0, 1.0, 1.0);
	Transform t = Transform(pos, rot, scale);
	fb.setTransform(t);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutCreateWindow("Fluid Simulation");

	glewInit();
	if (glewIsSupported("GL_VERSION_4_5")) {
		std::cout << " GLEW Version is 4.5\n ";
	}
	else {
		std::cout << "GLEW 4.5 not supported\n ";
	}

	initDraw();

	// register callbacks
	glutDisplayFunc(renderScene);
	glutReshapeFunc(reshape);
	glutIdleFunc(idle);
	glutKeyboardFunc(key);

	glutInitWindowSize(1280, 720);
	glutReshapeWindow(1280, 720);


	glutMainLoop();

	return 0;
}

void initDraw(void) {
	glEnable(GL_DEPTH_TEST);

}

//reshape callback function - called when the window size changed
void reshape(int width, int height) {
	//set the viewport to be the same width and height as the window
	glViewport(0, 0, width, height);
	//make changes to the projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	//set up our projection type
	//we'll be using a perspective projection, with a 90 degree 
	//field of view
	gluPerspective(45.0, (float)width / (float)height, 1.0, 1000.0);
	//redraw the view during resizing
	renderScene();
}

void renderScene(void) {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//Draw here
	fb.render();

	glFlush();

	glutSwapBuffers();
}

void idle(void) {
	std::chrono::high_resolution_clock::time_point curTime = std::chrono::high_resolution_clock::now();
	float dt = std::chrono::duration_cast<std::chrono::duration<float>>(curTime - lastTime).count();
	lastTime = curTime;

	fb.step(dt); //for some reason 1.0 instead of dt works

	glutPostRedisplay();
}

void key(unsigned char k, int x, int y) {
	switch (k) {
	case 'r':
		zRot++;
		quat rot = angleAxis(radians(zRot), vec3(0.0f, 1.0f, 0.0f));
		fb.getTransform()->setRot(rot);
		break;
	case 27: //27 is the ASCII code for the ESCAPE key
		exit(0);
		break;
	}
	glutPostRedisplay();
}