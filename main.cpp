#include "include\glew\glew.h"
#include "include\GLFW\glfw3.h"
#include "include\glm\glm.hpp"

#include "include\glm\gtc\matrix_transform.hpp"

#include <iostream>

#include "shader.h"
#include "fluid.h"

constexpr unsigned int SCRN_W = 720;
constexpr unsigned int SCRN_H = 720;
constexpr float ROTATE_STEP = 2.5;

//Simulation paramaters
constexpr unsigned int BOX_W = 200; //Note that making the box too large will result in the frame buffer object not being able to be created
constexpr unsigned int BOX_H = 200;
constexpr unsigned int BOX_D = 200;
constexpr unsigned int JACOBI_ITERATIONS = 40;
constexpr float DISSIPATION = 1.0f;
constexpr float BOUYANCY = 300.0f;
constexpr float WEIGHT = 3.0f;

using namespace glm;

mat4 view;
mat4 projection;
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
void keyEvent(GLFWwindow* window, int key, int scancode, int action, int mods);

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
	glfwSetKeyCallback(window, keyEvent);
	//glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);

	// Create the VAO:
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

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

	// Set up the vertex layout:
	GLsizeiptr stride = 2 * sizeof(positions[0]);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_SHORT, GL_FALSE, stride, 0);

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
	// Projection matrix : 45° Field of View, 16:9 ratio, display range : 0.0 unit <-> 100 units
	projection = glm::perspective(glm::radians(45.0f), (float)SCRN_W / (float)SCRN_H, 0.0f, 100.0f);

	// Or, for an ortho camera :
	//glm::mat4 Projection = glm::ortho(-10.0f,10.0f,-10.0f,10.0f,0.0f,100.0f); // In world coordinates

	// Camera matrix
	view = glm::lookAt(
		glm::vec3(0, 0, 3.5), // Camera is at (4,3,3), in World Space
		glm::vec3(0, 0, 0), // and looks at the origin
		glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
	);
}

void update() {
	float curTime = glfwGetTime();
	float dt = curTime - lastTime;
	lastTime = curTime;

	fluid.update(dt, quadVBO);
}

void draw() {
	glClear(GL_COLOR_BUFFER_BIT);

	// Draw
	fluid.render(cubeCenterVBO, view, projection, SCRN_W, SCRN_H);

	// Swap buffers
	glfwSwapBuffers(window);
}

void keyEvent(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS || action == GLFW_REPEAT)
		switch (key) {
			case GLFW_KEY_Q:
				fluid.getTransform().rotate(angleAxis(radians(ROTATE_STEP), vec3(1.0f, 0.0f, 0.0f)));
				break;
			case GLFW_KEY_A:
				fluid.getTransform().rotate(angleAxis(radians(-ROTATE_STEP), vec3(1.0f, 0.0f, 0.0f)));
				break;
			case GLFW_KEY_W:
				fluid.getTransform().rotate(angleAxis(radians(ROTATE_STEP), vec3(0.0f, 1.0f, 0.0f)));
				break;
			case GLFW_KEY_S:
				fluid.getTransform().rotate(angleAxis(radians(-ROTATE_STEP), vec3(0.0f, 1.0f, 0.0f)));
				break;
			case GLFW_KEY_E:
				fluid.getTransform().rotate(angleAxis(radians(ROTATE_STEP), vec3(0.0f, 0.0f, 1.0f)));
				break;
			case GLFW_KEY_D:
				fluid.getTransform().rotate(angleAxis(radians(-ROTATE_STEP), vec3(0.0f, 0.0f, 1.0f)));
				break;
			case GLFW_KEY_Z:
				fluid.toggleAuto();
				break;
			case GLFW_KEY_X:
				fluid.puff();
				break;
		}
}