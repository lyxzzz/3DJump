#include"camera.h"
#include"embedded.h"
#include<iostream>
using namespace std;
extern EmbeddedDevice mydevice;
extern GLfloat system_time;
inline float getTimeValuePercent(float value, float start, float last, float now)
{
	float tmp = (now - start) / last;
	return value * (tmp > 1.0f ? 1.0f : tmp);
}
void Camera::rotateView(GLfloat xoffset, GLfloat yoffset)
{
	this->xpos += xoffset;
	this->ypos += yoffset;
	if (this->ypos >= 1.0)
	{
		this->ypos = 1.0;
	}
	if (this->ypos <= -1.0)
	{
		this->ypos = -1.0;
	}
}
void Camera::resizeView(bool value)
{
	if (value) this->zpos -= 0.1f;
	else
		this->zpos += 0.1f;
	if (zpos < ZMIN)
		zpos = ZMIN;
	if (zpos > ZMAX)
		zpos = ZMAX;
}
void Camera::initView()
{
	this->xpos = 0.0f;
	this->ypos = 0.0f;
}
glm::mat4 Camera::getView()
{
		this->cameraView = glm::vec3(this->zpos*this->radius);
		GLfloat yaw = -this->xrange*this->xpos;
		GLfloat pitch = -this->yrange*this->ypos;
		yaw += (mydevice.yaw+getTimeValuePercent(mydevice.d_yaw,mydevice.s_time,mydevice.d_time,system_time));
		pitch += (mydevice.pitch + getTimeValuePercent(mydevice.d_pitch, mydevice.s_time, mydevice.d_time, system_time));
		mydevice.gyroscope_state = false;
		//this->xpos = -yaw / this->xrange;
		//this->ypos = -pitch / this->yrange;
		this->rotateRadians = glm::vec3(yaw, pitch, this->zpos);
		this->cameraView.x *= sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		this->cameraView.y *= sin(glm::radians(pitch));
		this->cameraView.z *= cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		return glm::lookAt(this->viewposition + cameraView, this->viewposition + glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		//cout << "h" << endl;
		//this->cameraView = glm::vec3(this->zpos*this->radius);
		//GLfloat yaw = -this->xrange*this->xpos;
		//GLfloat pitch = -this->yrange*this->ypos;
		//this->rotateRadians = glm::vec3(yaw, pitch, this->zpos);
		//this->cameraView.x *= sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		//this->cameraView.y *= sin(glm::radians(pitch));
		//this->cameraView.z *= cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		//return glm::lookAt(this->viewposition + cameraView, this->viewposition + glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
}
glm::vec3 Camera::getViewPos()
{
	return this->cameraView;
}
glm::vec3 Camera::getRoatateRadians()
{
	return this->rotateRadians;
}