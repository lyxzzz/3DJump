#ifndef SHADER1_H
#define SHADER1_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#define GLEW_STATIC
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader
{
public:
	GLuint Program;
	// Constructor generates the shader on the fly
	Shader(const GLchar* vertexPath, const GLchar* fragmentPath, const GLchar* geometryPath);
	Shader(const GLchar* vertexPath, const GLchar* fragmentPath);
	// Uses the current shader
	GLuint Use()
	{
		glUseProgram(this->Program);
		return this->Program;
	}
	void change1f(const char* name, GLfloat value)
	{
		glUniform1f(glGetUniformLocation(this->Program, name), value);
	}
	void change1i(const char* name, GLuint value)
	{
		glUniform1i(glGetUniformLocation(this->Program, name), value);
	}
	void changeMat4(const char* name, const glm::mat4& value)
	{
		glUniformMatrix4fv(glGetUniformLocation(this->Program, name), 1, GL_FALSE, glm::value_ptr(value));
	}
};

#endif