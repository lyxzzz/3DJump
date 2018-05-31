#include "callback.h"
#include<soil/SOIL.h>
#include"embedded.h"
GLfloat lastx = WIDTH / 2, lasty = WIDTH / 2;
bool firstdrag = false;
extern GLfloat system_time;
extern GLfloat borning_time;
extern EmbeddedDevice mydevice;
CallbackInfo::CallbackInfo(Camera* camera)
{
	this->lastTime = 0.0f;
	this->flyTime = 0.0f;
	this->camera = camera;
	this->cursor_state = false;
	this->fix_cursor = false;
	this->cursor_drag = false;
	this->followup = true;
	this->close = false;
	this->playerstate = ModelState::Generating;
	unsigned char pixels[CURSOR_HEIGHT * CURSOR_WIDTH * 4];
	memset(pixels, 0x00, sizeof(pixels));
	cursorimage.width = CURSOR_WIDTH;
	cursorimage.height = CURSOR_HEIGHT;
	cursorimage.pixels = pixels;
	cursor = glfwCreateCursor(&cursorimage, 0, 0);
}
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
		callback.close = true;
	}
	if (action == GLFW_PRESS && callback.playerstate == ModelState::Pause)
	{
		callback.playerstate = ModelState::Borning;
		borning_time = system_time;
		callback.camera->move(VIEW_INIT);
	}
	if (key == GLFW_KEY_Q && action == GLFW_RELEASE && callback.playerstate == ModelState::Stillness)
	{
		callback.playerstate = ModelState::Reborn;
	}
	if (callback.playerstate != ModelState::Accumlating && callback.playerstate != ModelState::Flying)
	{
		if (key == GLFW_KEY_GRAVE_ACCENT && action == GLFW_RELEASE)
		{
			if (callback.cursor_state)
			{
				callback.cursor_state = false;
				glfwDestroyCursor(callback.cursor);
				callback.fix_cursor = false;
			}
			else
			{
				callback.cursor_state = true;
				callback.cursor = glfwCreateCursor(&callback.cursorimage, 0, 0);
				glfwSetCursor(window, callback.cursor);
				callback.fix_cursor = true;
				mouse_callback(window, WIDTH / 2, HEIGHT / 2);
			}
		}
	}
	if (key == GLFW_KEY_Q && action == GLFW_RELEASE && callback.playerstate == ModelState::Flying)
	{
		callback.playerstate = ModelState::Borning;
		borning_time = system_time;
		callback.camera->move(VIEW_INIT);
	}
	if (key == GLFW_KEY_TAB && action == GLFW_RELEASE)
	{
		if (callback.cursor_state)
		{
			if (callback.playerstate == ModelState::Flying)
				callback.followup = !callback.followup;
			else
				callback.camera->initView();
		}
	}
}
void mouse_callback(GLFWwindow* window, double x, double y)
{
	if (callback.fix_cursor)
	{
		glfwSetCursorPos(window, WIDTH / 2, HEIGHT / 2);
		GLfloat xoffset = float((x - WIDTH / 2) / WIDTH) * 2;
		GLfloat yoffset = float(0 - (y - HEIGHT / 2) / HEIGHT) * 2;
		if (callback.playerstate == ModelState::Flying&&callback.followup)
			callback.camera->rotateView(xoffset/FLYING_RESISTENCE, yoffset);
		else
			callback.camera->rotateView(xoffset, yoffset);
	}
	else if (callback.cursor_drag)
	{
		if (firstdrag)
		{
			lastx = x;
			lasty = y;
			firstdrag = false;
		}
		//glfwSetCursorPos(window, WIDTH / 2, HEIGHT / 2);
		GLfloat xoffset = float((x - lastx) / WIDTH) * 2;
		GLfloat yoffset = float(0 - (y - lasty) / HEIGHT) * 2;
		lastx = x;
		lasty = y;
		callback.camera->rotateView(xoffset, yoffset);
	}
	return;
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	callback.camera->resizeView(yoffset > 0);
	return;
}
void mousebutton_callback(GLFWwindow* window, int state, int x, int y)
{
	if (!callback.fix_cursor)
	{
		firstdrag = true;
		callback.cursor_drag = (state == GLFW_MOUSE_BUTTON_LEFT) && (x == GLFW_PRESS);
	}
	else
	{
		if (state == GLFW_MOUSE_BUTTON_LEFT)
		{
			if (x == GLFW_PRESS && callback.playerstate == ModelState::Stillness)
			{
				callback.lastTime = system_time;
				callback.playerstate = ModelState::Accumlating;
			}
			if (x == GLFW_RELEASE && callback.playerstate == ModelState::Accumlating)
			{
				callback.playerstate = ModelState::Lauching;
				callback.flyTime = system_time + EJECTION_TIME / PERIOD_TIMES / 4;
			}
		}
	}
}