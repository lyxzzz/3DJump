#ifndef MODEL_H_
#define MODEL_H_

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <vector>
#include <string>

#include "Mesh.h"
#include "const.h"
using namespace std;
struct MyVertex {
	glm::vec3 Position;
	glm::vec2 TexCoords;
};
class Model {
public:
	virtual void Draw(Shader shader) = 0;
};
class ExternalModel:public Model{
public:
	ExternalModel(const char* path, bool gamma = false):gammaCorrection(gamma) {
		loadModel(path);
	}
	void Draw(Shader shader);

private:
	std::vector<Mesh> meshes;
	std::string directory;
	std::vector<Texture> textures_loaded;
	bool gammaCorrection;

	void loadModel(std::string path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> loadMaterialtextures(aiMaterial* mat, aiTextureType type, std::string typeName);
};
class InnerModel :public Model {
public:
	InnerModel(GLfloat* vertices,GLuint length,std::vector<GLuint>&& textures);
	InnerModel(const GLchar* path, std::vector<GLuint>&& textures);
	void Draw(Shader shader);
private:
	GLuint length;
	std::vector<GLuint> textures;
	unsigned int VAO, VBO, EBO;     //‰÷»æª∑æ≥
	void setupModel(GLfloat* vertices);
};

#endif