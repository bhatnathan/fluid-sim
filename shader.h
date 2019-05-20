#pragma once

#include "include\glew\glew.h"
#include "include\glm\glm.hpp"

#include <string>

class Shader {
public:
	unsigned int id; //program id

	Shader(const GLchar* vertexPath, const GLchar* fragmentPath);

	void use();

	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;
	void setMatrix(const std::string& name, glm::mat4 &value) const;
private:
	void checkCompileErrors(unsigned int shader, std::string type);
};

