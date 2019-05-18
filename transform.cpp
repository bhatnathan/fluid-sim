#include "transform.h"

#include "include\glm\gtc\matrix_transform.hpp"
#include "include\glm\gtx\quaternion.hpp"
#include "include\glm\gtc\type_ptr.hpp"
#include "include\freeglut\freeglut.h"


Transform::Transform() {
	this->pos = vec3();
	this->rot = angleAxis(radians(0.0f), vec3(1.0f, 0.0f, 0.0f)); //no rotation around arbitrary x axis
	this->scale = vec3(1.0, 1.0, 1.0);
}

Transform::~Transform() {
}

vec3 Transform::getPos() {
	return this->pos;
}
quat Transform::getRot() {
	return this->rot;
}

vec3 Transform::getScale() {
	return this->scale;
}

mat4 Transform::getMatrix() {
	mat4 res = mat4(1.0);
	res = glm::translate(res, pos);
	res = res * toMat4(rot);
	res = glm::scale(res, scale);

	return res;
}

void Transform::setPos(vec3 pos) {
	this->pos = pos;
}

void Transform::setRot(quat rot) {
	this->rot = rot;
}

void Transform::setScale(vec3 scale) {
	this->scale = scale;
}

void Transform::setMatrix() {
	float* mat = (float*)glm::value_ptr(this->getMatrix());
	glLoadMatrixf(mat);
}