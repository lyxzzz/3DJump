#include<Windows.h>
#include <direct.h>

#include <iostream>
#include <map>
#include<vector>
#include"util.h"
#include<time.h>
#include<stdlib.h>
#include"object.h"
#include"skybox.h"
#include<serial\serial.h>
#include<ft2build.h>
#include FT_FREETYPE_H
#include"mode.h"
#include"embedded.h"
#if VERSION==2
//#pragma comment(linker,"/subsystem:\"windows\" /entry:\"mainCRTStartup\"")
unsigned int loadTexture(char const * path);

struct Character {
	GLuint TextureID;   // ID handle of the glyph texture
	glm::ivec2 Size;    // Size of glyph
	glm::ivec2 Bearing;  // Offset from baseline to left/top of glyph
	GLuint Advance;    // Horizontal offset to advance to next glyph
};

std::map<GLchar, Character> Characters;
GLuint VAO, VBO;
void loadFont();
void RenderText(Shader &shader, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);
// 屏幕宽高
const unsigned int SCR_WIDTH = WIDTH;
const unsigned int SCR_HEIGHT = HEIGHT;

float factor = 0.2f;
float fov = 45.0f;

//摄像机
Camera camera;
CallbackInfo callback(&camera);
Object cursor;
GLfloat system_time;
GLuint platform_nums = 7;
vector<glm::vec3> modelsize(7);
ObjectList platform_list;
GLfloat borning_time = 0.0f;
GLfloat generating_time;
GLuint score = 0;
GLuint fps_times = 0;
GLdouble fps_time = 0.0f;
string fps_str;
EmbeddedDevice mydevice = { false,0.0f,0.0f,0.0f,0.0f,0.0f };
DWORD WINAPI ThreadProc(LPVOID lpParameter)
{
#if DEVICE_TEST
	ifstream f("test/testfile");
#else
	GLfloat data_time = 0.0f;
	GLfloat d_data_time = 0.0f;
	serial::Serial myport;
	myport.setPort("COM3");
	try {
		myport.open();
	}
	catch (std::exception & e)
	{
		cout << "Fail to open COM3" << endl;
		return 0;
	}
	if (!myport.isOpen())
	{
		cout << "Fail to open COM3" << endl;
		return 0;
	}
#endif
	for (size_t i = 0; i < INIT_TIMES; ++i)
	{
		try {
#if DEVICE_TEST
			string str;
			getline(f, str);
#else
			string str = myport.readline();
#endif

			GLfloat acc = 0.0f;
			GLfloat angle = 0.0f;
#if SSTREAM_TYPE
			size_t split = str.find_first_of(';');
			stringstream ss;
			ss << str.substr(0, split);
			ss >> acc;
			ss.clear();
			ss << str.substr(split + 1, str.size() - 1);
			ss >> angle;
#else
			sscanf_s(str.c_str(), "%f;%f\n", &acc, &angle);
#endif
		}
		catch (const std::exception& e)
		{
		}
	}
	while (true)
	{
		try {
#if DEVICE_TEST
			string str;
			getline(f, str);
#else
			string str = myport.readline();
#endif
#if SMOOTH
			mydevice.pitch += mydevice.d_pitch;
			mydevice.yaw += mydevice.d_yaw;
			mydevice.d_pitch = 0.0f;
			mydevice.d_yaw = 0.0f;
#endif
			GLfloat acc = 0.0f;
			GLfloat angle = 0.0f;
#if SSTREAM_TYPE
			size_t split = str.find_first_of(';');
			stringstream ss;
			ss << str.substr(0, split);
			ss >> acc;
			ss.clear();
			ss << str.substr(split + 1, str.size() - 1);
			ss >> angle;
#else
			sscanf_s(str.c_str(), "%f;%f\n", &acc, &angle);
#endif
			cout << str << endl;
			cout << "acc:" << acc << "\tangle:" << angle << endl;
			while (mydevice.gyroscope_state) {}
			if (distance(mydevice.pitch, angle) >= DISTANCE_VALUE)
			{
#if SMOOTH
				mydevice.d_pitch = angle - mydevice.pitch;
#else
				mydevice.pitch = angle;
#endif
			}
#if SMOOTH
			//mydevice.yaw += standardlizeAcc(acc);
			mydevice.d_yaw = -acc;
			mydevice.d_time = system_time - data_time;
			mydevice.s_time = data_time;
			data_time = system_time;
#else
			mydevice.yaw -= acc;
#endif
			mydevice.gyroscope_state = true;
		}
		catch (const std::exception& e)
		{
		}
	}

	return 0;
}
int main() {
	/*if (!open("COM3"))
	{
	cout << "Fail to open COM3" << endl;
	exit(0);
	}
	message m;
	GLuint message_size;*/
	HANDLE hThread1 = CreateThread(NULL, 0, ThreadProc, 0, 0, NULL);
	modelsize[Object::Cat] = glm::vec3(CAT_LENGTH, CAT_HEIGHT, CAT_WIDTH);
	modelsize[Object::Muppet] = glm::vec3(MUPPET_LENGTH, MUPPET_HEIGHT, MUPPET_WIDTH);
	modelsize[Object::Sphere] = glm::vec3(SPHERE_LENGTH, SPHERE_HEIGHT, SPHERE_WIDTH);
	modelsize[Object::Normal] = glm::vec3(1.0f, 1.0f, 1.0f);
	modelsize[Object::Cursor] = glm::vec3(CURSOR_SIZE);
	modelsize[Object::Nanosuit] = glm::vec3(NANOSUIT_LENGTH, NANOSUIT_HEIGHT, NANOSUIT_WIDTH);
	modelsize[Object::Circle] = glm::vec3(CIRCLE_LENGTH, CIRCLE_HEIGHT, CIRCLE_WIDTH);
	GLfloat flying_time = 0.0f;

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Lighting", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		return -1;
	}
	glfwMakeContextCurrent(window);
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetMouseButtonCallback(window, mousebutton_callback);
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	glewInit();

	// Define the viewport dimensions
	glViewport(0, 0, WIDTH, HEIGHT);

	//开启深度测试
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	//开启混合
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//！创建着色器
	Shader cubeshader("shader/formal/normal.vertex", "shader/formal/normal.frag", "shader/formal/normal.gs");
	Shader double_cubeshader("shader/formal/normal.vertex", "shader/formal/d_normal.frag", "shader/formal/normal.gs");
	Shader modelshader("shader/formal/model_loading.vertex", "shader/formal/model_loading.frag", "shader/formal/model_loading.gs");
	Shader sphereshader("shader/formal/sphere.vertex", "shader/formal/sphere.frag", "shader/formal/sphere.gs");
	Shader circleshader("shader/formal/circle.vertex", "shader/formal/circle.frag", "shader/formal/circle.gs");
	Shader cursorShader("shader/formal/cursor.vertex", "shader/formal/cursor.frag");
	Shader textShader("shader/formal/text.vertex", "shader/formal/text.frag");
	glm::mat4 textprojection = glm::ortho(0.0f, 1280.0f, 0.0f, 720.0f);
	textShader.Use();
	textShader.changeMat4("projection", textprojection);
	loadFont();


	// Configure VAO/VBO for texture quads
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);


	GLuint cubeTexture = loadTexture("photo/container.jpg");
	GLuint cursorTexture = loadTexture("photo/ring.png");
	GLuint t1 = loadTexture("photo/1.png");
	GLuint t2 = loadTexture("photo/2.png");
	GLuint t3 = loadTexture("photo/3.png");
	GLuint t4 = loadTexture("photo/4.png");
	GLuint t5 = loadTexture("photo/5.png");
	GLuint t6 = loadTexture("photo/6.jpg");

	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	glm::mat4&& rawview = camera.getView();
	glm::mat4* view = new glm::mat4(rawview);

	ExternalModel catmodel("model/cat/file.obj");
	ExternalModel muppetmodel("model/muppet/file.obj");
	ExternalModel spheremodel("model/sphere/file.obj");
	//ExternalModel nanosuitmodel("model/nanosuit/nanosuit.obj");
	//ExternalModel circlemodel("model/circle/file.obj");

	InnerModel cursormodel("data/cursor.data", { cursorTexture });
	InnerModel cubemodel1("data/cube.data", { t6 });
	InnerModel cubemodel2("data/cube.data", { cubeTexture });
	InnerModel cubemodel3("data/cube.data", { t1 });
	InnerModel cubemodel4("data/cube.data", { t2 });
	InnerModel cubemodel5("data/cube.data", { t3 });
	InnerModel cubemodel6("data/cube.data", { t4 });
	InnerModel cubemodel7("data/cube.data", { t5 });

	//Object o(&mymodel, &modelshader, glm::vec3(0.0f, -1.0f, -1.0f), 0.0f, view, projection);
	Object cursor(&cursormodel, &cursorShader, glm::vec3(0.0f), 0.0f, view, projection, Object::Cursor);
	//Object cube(&cubemodel, &cubeshader, OBJECT_OFFSET, 0.0f, view, projection, Object::Normal, glm::vec3(2.0f, 1.0f, 2.0f));
	Object cube(&cubemodel2, &cubeshader, OBJECT_OFFSET, 0.0f, view, projection, Object::Normal, glm::vec3(2.0f, 1.0f, 2.0f));
	Object cube1(&cubemodel1, &cubeshader, OBJECT_OFFSET, 0.0f, view, projection, Object::Normal, glm::vec3(2.0f, 1.0f, 2.0f));
	Object cube2(&cubemodel3, &cubeshader, OBJECT_OFFSET, 0.0f, view, projection, Object::Normal, glm::vec3(2.0f, 1.0f, 2.0f));
	Object cube3(&cubemodel4, &cubeshader, OBJECT_OFFSET, 0.0f, view, projection, Object::Normal, glm::vec3(2.0f, 1.0f, 2.0f));
	Object cube4(&cubemodel5, &cubeshader, OBJECT_OFFSET, 0.0f, view, projection, Object::Normal, glm::vec3(2.0f, 1.0f, 2.0f));
	Object cube5(&cubemodel6, &cubeshader, OBJECT_OFFSET, 0.0f, view, projection, Object::Normal, glm::vec3(2.0f, 1.0f, 2.0f));
	Object cube6(&cubemodel7, &cubeshader, OBJECT_OFFSET, 0.0f, view, projection, Object::Normal, glm::vec3(2.0f, 1.0f, 2.0f));
	//Object cube(&circlemodel, &circleshader, glm::vec3(0.0f,0.0f,0.0f), 0.0f, view, projection, Object::Circle);
	moving_trail cube_trail = [](GLfloat time) {return glm::vec3(0.0f, 0.0f, 0.0f); };
	Object player(&muppetmodel, &modelshader, MUPPET_POSITION, MUPPET_RADIANS, view, projection, Object::Muppet);
	Object shield(&spheremodel, &sphereshader, SHIELD_POSITION, SHIELD_RADIANS, view, projection, Object::Sphere);
	Skybox skybox("photo/sor_cwd/", ".jpg");
	float currentFrame;
	float lastFrame;
	float d;
	//主循环
	generating_time = glfwGetTime();
	system_time = generating_time;
	GLfloat player_angle = 0.0f;
	GLfloat shield_value = 0.0f;
	glm::vec3 offset(0.0f);
	//shield.explode(EXPLODE_TIME);
	shield.translucent(0.0f);
	platform_list.addTemplate(cube);
	platform_list.addTemplate(cube1);
	platform_list.addTemplate(cube2);
	platform_list.addTemplate(cube3);
	platform_list.addTemplate(cube4);
	platform_list.addTemplate(cube5);
	platform_list.addTemplate(cube6);

	platform_list.generatePlatform(platform_nums);


	while (!glfwWindowShouldClose(window)) {
		//每一帧的时间逻辑
		system_time = glfwGetTime();
		//message_size = read(&m);
		//cout << message_size << endl;
		//cout << m.buf[0] << endl;

		//std::cout << currentFrame << std::endl;
		glfwPollEvents();
		glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		*view = camera.getView();
		skybox.draw();
		if (callback.cursor_state) {
			cursor.resize(glm::vec3(camera.rotateRadians.z));
			cursor.draw(camera.viewposition, { { camera.rotateRadians.x, glm::vec3(0.0f, 1.0f, 0.0f) },{ -camera.rotateRadians.y,glm::vec3(1.0f,0.0f,0.0f) } });
		}
		switch (callback.playerstate)
		{
		case ModelState::Borning:
		{
			shield.translucent(0.0f);
			GLfloat time = system_time - borning_time;
			if (time >= BORNING_TIME)
			{
				time = BORNING_TIME;
				callback.playerstate = ModelState::Stillness;
			}
			cube.draw();
			player.draw(time);
			platform_list.draw();
			shield.draw();
			break;
		}
		case ModelState::Reborn:
		{
			callback.playerstate = ModelState::Generating;
			platform_list.generatePlatform(platform_nums);
			camera.move(VIEW_INIT);
			generating_time = system_time;
			shield.translucent(0.0f);
			break;
		}
		case ModelState::Generating:
		{
			GLfloat time = system_time - generating_time;
			if (time >= BORNING_TIME)
			{
				time = BORNING_TIME;
				callback.playerstate = ModelState::Stillness;
			}
			glm::vec3 moving = cube_trail(system_time);
			//cout << time << endl;
			cube.draw(moving);
			//camera.move(moving);
			if (!callback.cursor_drag)
			{
				player_angle = camera.rotateRadians.x;
			}
			player.draw(moving, { { player_angle,glm::vec3(0.0f,1.0f,0.0f) } });
			platform_list.draw(time);
			shield.draw(moving);
			break;
		}
		case ModelState::Stillness:
		{
			glm::vec3 moving = cube_trail(system_time);
			cube.draw(moving);
#if CAMERA_FOLLOWUP
			camera.move(moving);
#endif
			if (!callback.cursor_drag)
			{
				player_angle = camera.rotateRadians.x;
			}
			player.draw(moving, { { player_angle,glm::vec3(0.0f,1.0f,0.0f) } });
			platform_list.draw();
			shield.draw(moving);
			break;
		}

		case ModelState::Accumlating:
		{
			GLfloat time = system_time - callback.lastTime;
			if (time > MAX_INTENSITY)
			{
				time = MAX_INTENSITY;
			}
			glm::vec3 moving = cube_trail(system_time);
			GLfloat tmp = -cube.compress(time, moving);
			glm::vec3 move(0.0f, tmp, 0.0f);
#if CAMERA_FOLLOWUP
			camera.move(moving);
#endif
			//camera.move(moving);
			if (!callback.cursor_drag)
			{
				player_angle = camera.rotateRadians.x;
			}
			//player.draw(move + moving);
			shield_value = time / MAX_INTENSITY * 0.6 + 0.4;
			shield.translucent(shield_value);
			callback.view_speed = glm::vec3(-HORIZON_SPEED * sin(glm::radians(player_angle)), SPEED_POWER*time*time, -HORIZON_SPEED * cos(glm::radians(player_angle)));
			platform_list.draw();
			shield.draw(move+moving, { {-system_time*3600.0f,glm::vec3(1.0f,0.0f,0.0f)} });
			//cout << callback.view_speed.x << ":" << callback.view_speed.y << ":" << callback.view_speed.z << endl;
			break;
		}
		case ModelState::Lauching:
		{
			GLfloat time = system_time - callback.lastTime;
			platform_list.draw();
			glm::vec3 moving = cube_trail(system_time);
			offset = moving;
#if CAMERA_FOLLOWUP
			camera.move(moving);
#endif
			if (time > MAX_INTENSITY)
			{
				time = MAX_INTENSITY;
			}
			if (system_time >= callback.flyTime)
			{
				callback.playerstate = ModelState::Flying;
				flying_time = system_time;
			}
			cube.ejection(EJECTION_TIME, time);
			cube.draw(moving);
			if (!callback.cursor_drag)
			{
				player_angle = camera.rotateRadians.x;
			}
			GLfloat offsettime = system_time - callback.flyTime + EJECTION_TIME;
			glm::vec3 move(0.0f, cube.eject_func(offsettime), 0.0f);
			player.draw(move + moving, { { player_angle,glm::vec3(0.0f,1.0f,0.0f) } });
			shield.draw(move + moving);
			break;
		}
		case ModelState::Pause:
		{
			platform_list.draw();
			cube.draw();
			player.draw();
			shield.draw();
			RenderText(textShader, "You failed!", 320.0f, 400.0f, 3.0f, glm::vec3(1.0f, 0.0f, 0.0f));
			RenderText(textShader, "Press enter to reborn.", 125.0f, 200.0f, 2.5f, glm::vec3(1.0f, 0.0f, 0.0f));
			break;
		}
		case ModelState::Flying:
		{
			platform_list.draw();
			glm::vec3 moving = cube_trail(system_time);
			cube.draw(moving);
			GLfloat t = system_time - flying_time;
			GLfloat left_shield = shield_value - SHIELD_CONSUME * t;
			if (!callback.cursor_drag)
			{
				player_angle = camera.rotateRadians.x;
			}
			if (left_shield >= 0)
			{
				glm::vec3 move = t * callback.view_speed;
				move.y = callback.view_speed.y*t - 0.5f*VERTICAL_ACCELERATION*t*t;
				move += offset;
				//cout << move.x << ":" << move.y << ":" << move.z << endl;
				if (callback.followup)
				{
					camera.move(move);
				}
				else
				{
					camera.move(VIEW_INIT);
				}
				player.draw(move, { { player_angle,glm::vec3(0.0f,1.0f,0.0f) } });
				shield.translucent(left_shield);
				GLfloat rad = glm::radians(t*FLYING_ANGULAR);
				move.z -= sin(rad) / 2;
				move.y += ((1 - cos(rad))*0.58f);
				shield.draw(move, { { t*FLYING_ANGULAR,glm::vec3(1.0f,0.0f,0.0f) } });
				switch (platform_list.judgePosition(system_time, move + PLAYER_OFFSET, SPHERE_RADIUS))
				{
				case ObjectList::MovingResult::collision:
				{
					callback.playerstate = ModelState::Pause;
					//borning_time = system_time;
					//camera.move(VIEW_INIT);
					break;
				}
				case ObjectList::MovingResult::fly:
				{
					break;
				}
				case ObjectList::MovingResult::land:
				{
					cube = platform_list.land_object;
#if CUBE_MOVING
					cube_trail = platform_list.land_func;
#endif
					callback.playerstate = ModelState::Generating;
					platform_list.generatePlatform(platform_nums);
					camera.move(VIEW_INIT);
					generating_time = system_time;
					shield.translucent(0.0f);
					++score;
					break;
				}
				}
				if (callback.followup) callback.view_speed = glm::vec3(-HORIZON_SPEED * sin(glm::radians(player_angle)), callback.view_speed.y, -HORIZON_SPEED * cos(glm::radians(player_angle)));
				//shield.draw(move);
			}
			else
			{
				callback.playerstate = ModelState::Pause;
				//borning_time = system_time;
				//camera.move(VIEW_INIT);
			}
			break;
		}
		}

		stringstream ss;
		ss << "your score:";
		ss << score;
		//RenderText(textShader, "This is sample text", 25.0f, HEIGHT-100.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));
		RenderText(textShader, ss.str(), 500.0f, 650.0f, 1.0f, glm::vec3(0.3, 0.7f, 0.9f));
		if (system_time - fps_time >= 1.0f)
		{
			fps_time = system_time;
			stringstream().swap(ss);
			ss << "Fps:";
			ss << fps_times;
			fps_times = 0;
			fps_str = ss.str();
		}
		++fps_times;
		RenderText(textShader, fps_str, 25.0f, 700.0f, 0.5f, glm::vec3(1.0, 0.7f, 0.9f));
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}
//加载纹理
unsigned int loadTexture(char const * path) {
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char * data = SOIL_load_image(path, &width, &height, &nrComponents, 0);
	if (data) {
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		SOIL_free_image_data(data);
	}
	else {
		std::cout << "纹理加载失败，路径是:" << path << std::endl;
		SOIL_free_image_data(data);
	}

	return textureID;
}
void RenderText(Shader &shader, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color)
{
	// Activate corresponding render state	
	shader.Use();
	glUniform3f(glGetUniformLocation(shader.Program, "textColor"), color.x, color.y, color.z);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(VAO);

	// Iterate through all characters
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = Characters[*c];

		GLfloat xpos = x + ch.Bearing.x * scale;
		GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

		GLfloat w = ch.Size.x * scale;
		GLfloat h = ch.Size.y * scale;
		// Update VBO for each character
		GLfloat vertices[6][4] = {
			{ xpos,     ypos + h,   0.0, 0.0 },
		{ xpos,     ypos,       0.0, 1.0 },
		{ xpos + w, ypos,       1.0, 1.0 },

		{ xpos,     ypos + h,   0.0, 0.0 },
		{ xpos + w, ypos,       1.0, 1.0 },
		{ xpos + w, ypos + h,   1.0, 0.0 }
		};
		// Render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
		// Update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // Be sure to use glBufferSubData and not glBufferData

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// Render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}
void loadFont()
{
	FT_Library ft;
	// All functions return a value different than 0 whenever an error occurred
	if (FT_Init_FreeType(&ft))
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

	// Load font as face
	FT_Face face;
	if (FT_New_Face(ft, "font/Sofia.otf", 0, &face))
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;

	//// Set size to load glyphs as
	FT_Set_Pixel_Sizes(face, 0, 48);

	// Disable byte-alignment restriction
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	for (GLubyte c = 0; c < 128; c++)
	{
		// Load character glyph 
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}
		// Generate texture
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer
		);
		// Set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// Now store character for later use
		Character character = {
			texture,
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x
		};
		Characters.insert(std::pair<GLchar, Character>(c, character));
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	// Destroy FreeType once we're finished
	FT_Done_Face(face);
	FT_Done_FreeType(ft);
}
#endif