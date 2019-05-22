#include "include\glew\glew.h"
#include "include\GLFW\glfw3.h"
#include "include\glm\glm.hpp"

#include "include\glm\gtc\matrix_transform.hpp"

#include <iostream>

#include "shader.h"
#include "fluid.h"

constexpr unsigned int SCRN_W = 1280;
constexpr unsigned int SCRN_H = 720;

//Simulation paramaters
constexpr unsigned int BOX_W = 96; //Note that making the box too large will result in the frame buffer object not being able to be created
constexpr unsigned int BOX_H = 96;
constexpr unsigned int BOX_D = 96;
constexpr unsigned int JACOBI_ITERATIONS = 80;
constexpr float DISSIPATION = 1.0f;
constexpr float BOUYANCY = 300.0f;
constexpr float WEIGHT = 1.0f;

using namespace glm;

mat4 Model;
mat4 View;
mat4 Projection;
mat4 mvp;
GLuint cubeCenterVBO;
GLuint quadVBO;

GLFWwindow* window;

Fluid fluid;
float lastTime;

int initProgram();
void initSimulation();
void setUpMVP();
void update();
void draw();

int main(int argc, char** argv) {

	if (initProgram() == -1)
		return -1;

	setUpMVP();

	initSimulation();

	do {
		update();
		draw();
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

	glfwTerminate();

	return 0;
}

int initProgram() {
	if (!glfwInit()) {
		std::cerr << "FAILED TO INITIALIZE GLFW" << std::endl;
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // for macos
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL 

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(SCRN_W, SCRN_H, "Fluid Simulation", NULL, NULL);
	if (window == NULL) {
		std::cerr << "FAILED TO OPEN GLFW WINDOW" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window); // Initialize GLEW

	glewExperimental = true; // Needed in core profile
	if (glewInit() != GLEW_OK) {
		std::cerr << "FAILED TO INITIALIZE GLEW" << std::endl;
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	//Cube Center vbo
	float p[] = { 0, 0, 0 };
	glGenBuffers(1, &cubeCenterVBO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeCenterVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(p), &p[0], GL_STATIC_DRAW);

	//Set up quad
	short positions[] = {
		-1, -1,
		 1, -1,
		-1,  1,
		 1,  1,
	};

	// Create the VBO:
	GLsizeiptr size = sizeof(positions);
	glGenBuffers(1, &quadVBO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, size, positions, GL_STATIC_DRAW);

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnableVertexAttribArray(0);
	return 0;
}

void initSimulation() {
	fluid = Fluid(BOX_W, BOX_H, BOX_D, JACOBI_ITERATIONS, DISSIPATION, BOUYANCY, WEIGHT);
	lastTime = glfwGetTime();
}

void setUpMVP() {
	//SET UP MODEL VIEW AND PROJECTION MATRICES
	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	Projection = glm::perspective(glm::radians(45.0f), (float)16 / (float)9, 0.1f, 100.0f);

	// Or, for an ortho camera :
	//glm::mat4 Projection = glm::ortho(-10.0f,10.0f,-10.0f,10.0f,0.0f,100.0f); // In world coordinates

	// Camera matrix
	View = glm::lookAt(
		glm::vec3(4, 3, 3), // Camera is at (4,3,3), in World Space
		glm::vec3(0, 0, 0), // and looks at the origin
		glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
	);

	// Model matrix : an identity matrix (model will be at the origin)
	Model = glm::mat4(1.0f);
	// Our ModelViewProjection : multiplication of our 3 matrices
	mvp = Projection * View * Model; // Remember, matrix multiplication is the other way around
}

void update() {
	float curTime = glfwGetTime();
	float dt = curTime - lastTime;
	lastTime = curTime;

	fluid.update(dt);
}

void draw() {
	glClear(GL_COLOR_BUFFER_BIT);

	// Draw
	fluid.render(cubeCenterVBO);

	// Swap buffers
	glfwSwapBuffers(window);
}