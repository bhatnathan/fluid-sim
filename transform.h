#pragma once

#include "include\glm\glm.hpp"
#include "include\glm\gtc\quaternion.hpp"


using namespace glm;

class Transform {
public:
	Transform();
	Transform(vec3 pos, quat rot, vec3 scale) : pos(pos), rot(rot), scale(scale) {}
	~Transform();

	vec3 getPos();
	quat getRot();
	vec3 getScale();
	mat4 getMatrix();

	void setMatrix();
private:
	vec3 pos;
	quat rot;
	vec3 scale;
};

