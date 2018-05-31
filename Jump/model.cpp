#include "Model.h"
#include <glm/glm.hpp>

#include<soil/SOIL.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
using namespace std;

unsigned int TextureFromFile(const char *path, const std::string &directory, bool gamma = false);

//��Ⱦ
void ExternalModel::Draw(Shader shader) {
	for (int i = 0; i < meshes.size(); ++i)
		meshes[i].Draw(shader);
}

//����ģ��
void ExternalModel::loadModel(std::string path) {
	//�����scene����
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

	//����Ƿ�ɹ�
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cout << "Assimp����ģ��ʧ�ܣ�������Ϣ: " << importer.GetErrorString() << std::endl;
		return;
	}

	directory = path.substr(0, path.find_last_of('/'));

	processNode(scene->mRootNode, scene);
}

//����ڵ�
void ExternalModel::processNode(aiNode* node, const aiScene* scene) {
	//����ڵ������������Ϣ
	for (GLuint i = 0; i < node->mNumMeshes; ++i) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}

	//�����е��ӽڵ�����ͬ����
	for (GLuint i = 0; i < node->mNumChildren; ++i) {
		processNode(node->mChildren[i], scene);
	}
}

//��������
Mesh ExternalModel::processMesh(aiMesh* mesh, const aiScene* scene) {
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	for (GLuint i = 0; i < mesh->mNumVertices; ++i) {
		Vertex vertex;
		//�����㣬���ߺ���������
		glm::vec3 vector;
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.Position = vector;

		vector.x = mesh->mNormals[i].x;
		vector.y = mesh->mNormals[i].y;
		vector.z = mesh->mNormals[i].z;
		vertex.Normal = vector;

		if (mesh->mTextureCoords[0]) {	//�����Ƿ���������Ϣ
			glm::vec2 vec;
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = vec;
		}
		else
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);

		////����
		//vector.x = mesh->mTangents[i].x;
		//vector.y = mesh->mTangents[i].y;
		//vector.z = mesh->mTangents[i].z;
		//vertex.Tangent = vector;

		////��������
		//vector.x = mesh->mBitangents[i].x;
		//vector.y = mesh->mBitangents[i].y;
		//vector.z = mesh->mBitangents[i].z;
		//vertex.Bitangent = vector;

		vertices.push_back(vertex);
	}

	//��������
	for (GLuint i = 0; i < mesh->mNumFaces; ++i) {
		aiFace face = mesh->mFaces[i];
		for (GLuint j = 0; j < face.mNumIndices; ++j)
			indices.push_back(face.mIndices[j]);
	}

	//�������
	if (mesh->mMaterialIndex >= 0) {
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		std::vector<Texture> diffuseMaps = loadMaterialtextures(material,
			aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		std::vector<Texture> specularMaps = loadMaterialtextures(material,
			aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	}

	return Mesh(vertices, indices, textures);
}

std::vector<Texture> ExternalModel::loadMaterialtextures(aiMaterial* mat, aiTextureType type, std::string typeName) {
	std::vector<Texture> textures;
	for (GLuint i = 0; i < mat->GetTextureCount(type); ++i) {
		aiString str;
		mat->GetTexture(type, i, &str);

		bool skip = false;
		for (int j = 0; j < textures_loaded.size(); ++j) {

			if (std::strcmp(textures_loaded[j].path.c_str(), str.C_Str()) == 0) {
				textures.push_back(textures_loaded[j]);
				skip = true;
				break;
			}
		}

		if (!skip) {
			Texture texture;
			texture.id = TextureFromFile(str.C_Str(), directory);
			texture.type = typeName;
			texture.path = str.C_Str();
			textures.push_back(texture);
			textures_loaded.push_back(texture);
		}
	}

	return textures;
}
InnerModel::InnerModel(GLfloat* vertices, GLuint length, std::vector<GLuint>&& textures)
{
	this->length = length;
	(this->textures).insert((this->textures).end(), textures.begin(), textures.end());
	setupModel(vertices);
}
InnerModel::InnerModel(const GLchar* path, std::vector<GLuint>&& textures)
{
	ifstream file;
	//ȷ���ļ���������쳣
	file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	GLfloat buf[MAX_POINT_BUFFER];
	GLuint index = 0;
	try {
		//���ļ�
		file.open(path);
		stringstream ss;
		ss << file.rdbuf();
		file.close();
		while(!ss.fail())
		{
			ss >> buf[index++];
		}
	}
	catch (std::ifstream::failure e) {
		std::cout << "���󣺶�ȡ�ļ�ʧ�ܣ������ļ��Ƿ���ڣ�" << std::endl;
	}
	(this->textures).insert((this->textures).end(), textures.begin(), textures.end());
	this->length = (index - 1) * sizeof(GLfloat);
	setupModel(buf);
}
void InnerModel::setupModel(GLfloat* vertices) {
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	//glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, length, vertices, GL_STATIC_DRAW);

	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
	//	&indices[0], GL_STATIC_DRAW);

	//����λ��
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);


	//��������
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

	//�ָ�Ĭ�ϵ�VAO����
	glBindVertexArray(0);
}
void InnerModel::Draw(Shader shader)
{
	for (size_t i = 0; i < this->textures.size(); ++i)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, textures[i]);
		if (i == 0)
		{
			shader.change1i("texture1", i);
		}
		else if (i == 1)
		{
			shader.change1i("texture2", i);
		}

	}
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, length);
	glBindVertexArray(0);
	// Activate shader

}
unsigned int TextureFromFile(const char *path, const std::string &directory, bool gamma) {
	std::string filename = path;
	filename = directory + '/' + filename;
	std::cout << filename << std::endl;
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = SOIL_load_image(filename.c_str(), &width, &height, &nrComponents, 0);
	if (data) {
		GLenum format;
		if (nrComponents == 1)
			format = GL_RGB;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		SOIL_free_image_data(data);
	}
	else {
		std::cout << "�������ʧ�ܣ�·����" << path << std::endl;
		SOIL_free_image_data(data);
	}

	return textureID;
}