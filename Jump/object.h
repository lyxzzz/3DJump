#pragma once
#include<shader.h>
#include<model.h>
#include<functional>
extern GLfloat system_time;
struct Rotate
{
	GLfloat radians;
	glm::vec3 axis;
};
extern vector<glm::vec3> modelsize;
typedef glm::vec3(*rawmovingtrail)(GLfloat,GLfloat,GLfloat,GLfloat);
typedef function<glm::vec3(GLfloat)> moving_trail;
static auto ecjection_fun = [](GLfloat time, GLfloat length)->GLfloat {
	return -length * cos(glm::radians(time / EJECTION_TIME * PERIOD_TIMES*360.0f));
};
class Object
{
private:
	glm::mat4 translate;
	glm::mat4 rotate;
	glm::mat4* view;
	glm::mat4 projection;
	glm::vec3 size;
	GLfloat time;
	GLfloat transparency;
	GLfloat timer;
	GLfloat eject_timer;
	Shader* shader;
	Model* object;
	GLfloat compressunit;
	friend class ObjectList;
	void explode();
	GLfloat ejection();
public:
	enum ModelType
	{
		Cat,
		Muppet,
		Sphere,
		Normal,
		Cursor,
		Nanosuit,
		Circle
	};
public:
	function<GLfloat(GLfloat)> eject_func;
	ModelType object_type;
	Object() {}
	Object(Model* o, Shader* s, const glm::vec3& t, GLfloat r, glm::mat4* v, const glm::mat4& p,ModelType modeltype,glm::vec3 size=glm::vec3(1.0f,1.0f,1.0f)) :object(o), shader(s), projection(p), view(v),size(size),timer(0.0f),compressunit(size.y/MAX_INTENSITY/2.0f),eject_func(nullptr),eject_timer(0.0f),object_type(modeltype),time(0.0f),transparency(1.0f)
	{ 
		this->translate = glm::translate(this->translate, t);
		this->rotate = glm::rotate(this->rotate, glm::radians(r), glm::vec3(0.0f, 1.0f, 0.0f));
		if (modeltype == ModelType::Circle)
		{
			this->rotate = glm::translate(this->rotate, glm::vec3(0.0f, -0.5f, 0.0f));
			//this->rotate = glm::rotate(this->rotate, -90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		}
		this->rotate = glm::scale(this->rotate, modelsize[modeltype]);
	}
	Object(const Object& o)
	{
		this->object_type = o.object_type;
		this->eject_func = NULL;
		this->compressunit = o.compressunit;
		this->object = o.object;
		this->shader = o.shader;
		this->eject_timer = 0.0f;
		this->timer = 0.0f;
		this->size = o.size;
		this->projection = o.projection;
		this->view = o.view;
		this->rotate = o.rotate;
		this->translate = o.translate;
		this->time = o.time;
		this->transparency = o.transparency;
	}
	void reset(glm::vec3 position, glm::vec3 size = glm::vec3(1.0f, 1.0f, 1.0f))
	{ 
		this->eject_func = NULL;
		this->compressunit = size.y / MAX_INTENSITY / 2.0f;
		this->size = size;
		this->eject_timer = 0.0f;
		this->timer = 0.0f;
		this->time = 0.0f;
		this->transparency = 1.0f;
		this->translate = glm::translate(glm::mat4(), position + SHIELD_POSITION + glm::vec3(0.0f, -size.y / 2, 0.0f));
	}
	void draw(glm::vec3 trans);
	void draw();
	void draw(vector<Rotate>&& r);
	void draw(glm::vec3 trans,vector<Rotate>&& r);
	void draw(GLfloat borningtime);
	GLfloat compress(GLfloat time,glm::vec3 moving);
	void ejection(GLfloat time,GLfloat compresstime) {
		if (eject_timer < system_time) {
			eject_timer = system_time + time;
			eject_func = std::bind(ecjection_fun, std::placeholders::_1, compresstime*this->compressunit);
		}
	}
	void explode(GLfloat time) { if (timer < system_time)timer = system_time + time; }
	void resize(glm::vec3 size)
	{
		this->size = size;
	}
	void translucent(GLfloat percent) { this->shader->Use(); this->transparency = percent; this->shader->change1f("transparency", percent); }
};
class ObjectList
{
private:
	vector<Object> object_list;
	vector<moving_trail> func_list;
	vector<glm::vec3> buf_list;
	static vector<rawmovingtrail> rawfunc_list;
	vector<Object> template_list;
	GLuint size;
public:
	enum MovingResult{
		collision,
		land,
		fly,
	};
	Object land_object;
	moving_trail land_func;
	ObjectList():object_list(MAX_OBJECT), func_list(MAX_OBJECT),size(0),buf_list(MAX_OBJECT),template_list() {}
	MovingResult judgePosition(GLfloat time,glm::vec3 position, GLfloat radius);
	void addTemplate(const Object& o) { template_list.push_back(o); }
	void generatePlatform(GLuint size);
	void draw(GLfloat time);
	void draw();
};