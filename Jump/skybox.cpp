#include<skybox.h>
static 	float skyboxVertices[] = {
	// 位置  
	-1.0f, 1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f, 1.0f, -1.0f,
	-1.0f, 1.0f, -1.0f,

	-1.0f, -1.0f, 1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f, 1.0f, -1.0f,
	-1.0f, 1.0f, -1.0f,
	-1.0f, 1.0f, 1.0f,
	-1.0f, -1.0f, 1.0f,

	1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,

	-1.0f, -1.0f, 1.0f,
	-1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, -1.0f, 1.0f,
	-1.0f, -1.0f, 1.0f,

	-1.0f, 1.0f, -1.0f,
	1.0f, 1.0f, -1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f, -1.0f,

	-1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f, 1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f, 1.0f,
	1.0f, -1.0f, 1.0f
};
unsigned int loadCubemapsTexture(const char* dir, const char* suffix);
unsigned int loadCubemapsTexture(const std::vector<std::string>& fullpathVec);
extern Camera camera;
Skybox::Skybox(const char* dir, const char* suffix):skyboxShader("shader/formal/skybox.vertex", "shader/formal/skybox.frag")
{
	for (int i = 0; i < 108; ++i)
	{
		skyboxVertices[i] *= 40;
	}
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	loadCubemapsTexture("photo/sor_cwd/", ".jpg");
	skyboxShader.Use();
	glUniform1f(glGetUniformLocation(skyboxShader.Program, "skybox"), 0);
}
void Skybox::draw()
{
	glm::mat4 view = camera.getView();
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
	glm::mat4 model = glm::translate(glm::mat4(), camera.viewposition);
	glDepthMask(GL_FALSE);
	skyboxShader.Use();
	skyboxShader.changeMat4("model", model);
	skyboxShader.changeMat4("view", view);
	skyboxShader.changeMat4("projection", projection);
	glBindVertexArray(skyboxVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glDepthMask(0xFF);
}
unsigned int loadCubemapsTexture(const char* dir, const char* suffix)
{
	std::vector<std::string> faces;
	faces.push_back(std::string(dir) + "front" + std::string(suffix));
	faces.push_back(std::string(dir) + "back" + std::string(suffix));
	faces.push_back(std::string(dir) + "top" + std::string(suffix));
	faces.push_back(std::string(dir) + "bottom" + std::string(suffix));
	faces.push_back(std::string(dir) + "right" + std::string(suffix));
	faces.push_back(std::string(dir) + "left" + std::string(suffix));

	return loadCubemapsTexture(faces);
}
unsigned int loadCubemapsTexture(const std::vector<std::string>& fullpathVec) {
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrComponents;
	for (GLuint i = 0; i < fullpathVec.size(); ++i) {
		unsigned char * data = SOIL_load_image(fullpathVec[i].c_str(), &width, &height, &nrComponents, 0);
		if (data) {
			GLenum format;
			if (nrComponents == 1)
				format = GL_RED;
			else if (nrComponents == 3)
				format = GL_RGB;
			else if (nrComponents == 4)
				format = GL_RGBA;

			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

			SOIL_free_image_data(data);
		}
		else {
			std::cout << "纹理加载失败，路径是:" << fullpathVec[i] << std::endl;
			SOIL_free_image_data(data);
		}
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_REPEAT);


	return textureID;
}