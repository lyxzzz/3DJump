#ifndef CALLBACK_H
#define CALLBACK
#include"camera.h"
#include <GLFW/glfw3.h>
class CallbackInfo {
public:
	Camera * camera;
	bool cursor_state;
	bool fix_cursor;
	bool cursor_drag;
	bool followup;
	bool close;
	ModelState playerstate;
	glm::vec3 view_speed;
	GLfloat lastTime;
	GLfloat flyTime;
	GLFWcursor* cursor;
	GLFWimage cursorimage;
	CallbackInfo(Camera* camera);
};
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double x, double y);
void scroll_callback(GLFWwindow* window, double x, double y);
void mousebutton_callback(GLFWwindow* window, int state, int x, int y);
extern CallbackInfo callback;
#endif // !CALLBACK_H
