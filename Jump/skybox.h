#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <vector>
#include "Shader.h"
#include "callback.h"
#include<soil/SOIL.h>
class Skybox
{
private:
	GLuint skyboxVAO, skyboxVBO, texture;
	Shader skyboxShader;
public:
	Skybox(const char* dir, const char* suffix);
	void draw();
};