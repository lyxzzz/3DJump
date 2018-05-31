#ifndef CAMERA_H
#define CAMERA_H
#define GLEW_STATIC
#include <GL\glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


class Camera
{
private:
	GLfloat lastx, lasty, xpos, ypos, zpos;
public:
	Camera(GLuint width, GLuint height) :lastx(width/2),lasty(height/2),xpos(0),ypos(0),zpos(0){}
};
#endif
