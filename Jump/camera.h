#ifndef CAMERA_H
#define CAMERA_H
#define GLEW_STATIC
#include <GL\glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include"const.h"

class Camera
{
private:
	GLfloat lastx, lasty, xpos, ypos, zpos, radius,xrange,yrange;
public:
	glm::vec3 viewposition;
	glm::vec3 cameraView, rotateRadians;
	Camera(GLuint width=WIDTH, GLuint height=HEIGHT,GLfloat r=RADIUS,GLfloat xran=XRAN,GLfloat yran=YRAN):lastx(width / 2), lasty(height / 2), xpos(0), ypos(0), zpos(1.0f),radius(r),xrange(xran),yrange(yran),cameraView(),rotateRadians(),viewposition() {}
	void rotateView(GLfloat xoffset, GLfloat yoffset);
	void resizeView(bool add);
	void initView();
	glm::mat4 getView();
	glm::vec3 getViewPos();
	glm::vec3 getRoatateRadians();
	void move(glm::vec3 translate) { viewposition = translate; }
};
#endif
