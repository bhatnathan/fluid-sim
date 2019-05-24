#pragma once

#include "include\glew\glew.h"
#include "include\glm\glm.hpp"

#include <string>

//A class to hold a shader program, most of this code is taken from an online tutorial (with some minor adjustments)
class Shader {
public:
	unsigned int id; //program id

	Shader();

	Shader(const GLchar* vertexPath, const GLchar* fragmentPath);
	Shader(const GLchar* vertexPath, const GLchar* geometryPath, const GLchar* fragmentPath); //I wrote this function myself

	void use();

	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;
	void setVec2(const std::string& name, glm::vec2 value) const; //I wrote this
	void setVec3(const std::string& name, glm::vec3 value) const; //I wrote this
	void setMatrix(const std::string& name, glm::mat4 &value) const; //I wrote this
private:
	void checkCompileErrors(unsigned int shader, std::string type);
};

