#include"object.h"
#include"util.h"
#include<time.h>
struct CubeEquation
{
	glm::vec3 min;
	glm::vec3 max;
};
inline GLfloat getNearest(GLfloat offset, GLfloat cubemin, GLfloat cubemax)
{
	return offset <= cubemin ? (cubemin - offset) : (offset >= cubemax ? (cubemax - offset) : 0.0f);
}
ObjectList::MovingResult getAABB(glm::vec3 sphere_center, GLfloat radius, CubeEquation&& cube);
void generatePoint(GLuint size, vector<glm::vec3>& buf);
glm::vec3 generateSize();
GLfloat getRadians();
void Object::draw()
{
	shader->Use();
	if (eject_timer <= system_time)
	{
		shader->changeMat4("model", translate*glm::scale(rotate, size));
	}
	else
	{
		GLfloat offset = this->ejection();
		glm::mat4 tmp_trans = glm::translate(translate, glm::vec3(0.0f, offset / 2, 0.0f));
		glm::mat4 tmp_rot = glm::scale(rotate, glm::vec3(size.x, size.y + offset, size.z));
		shader->changeMat4("model", tmp_trans*tmp_rot);
	}
	shader->changeMat4("view", *view);
	shader->changeMat4("projection", projection);
	shader->change1f("time", this->time);
	shader->change1f("transparency", this->transparency);
	this->explode();
	object->Draw(*shader);
}
void Object::draw(glm::vec3 trans)
{
	shader->Use();
	if (eject_timer <= system_time)
	{
		shader->changeMat4("model", glm::translate(translate, trans)*glm::scale(rotate, size));
	}
	else
	{
		GLfloat offset = this->ejection();
		trans.y += offset / 2;
		glm::mat4 tmp_trans = glm::translate(translate, glm::vec3(0.0f, offset / 2, 0.0f));
		glm::mat4 tmp_rot = glm::scale(rotate, glm::vec3(size.x, size.y + offset, size.z));
		shader->changeMat4("model", glm::translate(translate, trans)*tmp_rot);
	}
	shader->changeMat4("view", *view);
	shader->changeMat4("projection", projection);
	shader->change1f("time", this->time);
	shader->change1f("transparency", this->transparency);
	this->explode();
	object->Draw(*shader);
}
void Object::draw(vector<Rotate>&& r)
{
	shader->Use();
	glm::mat4 tmpmodel = rotate;
	for (size_t i=0;i<r.size();++i)
	{
		tmpmodel = glm::rotate(tmpmodel, glm::radians(r[i].radians), r[i].axis);
	}
	if (eject_timer <= system_time)
	{
		tmpmodel = glm::scale(tmpmodel, size);
		shader->changeMat4("model", translate*tmpmodel);
	}
	else
	{
		GLfloat offset = this->ejection();
		glm::mat4 tmp_trans = glm::translate(translate, glm::vec3(0.0f, offset / 2, 0.0f));
		tmpmodel = glm::scale(tmpmodel, glm::vec3(size.x, size.y + offset, size.z));
		shader->changeMat4("model", tmp_trans*tmpmodel);
	}
	shader->changeMat4("view", *view);
	shader->changeMat4("projection", projection);
	shader->change1f("time", this->time);
	shader->change1f("transparency", this->transparency);
	this->explode();
	object->Draw(*shader);
}
void Object::draw(glm::vec3 trans,vector<Rotate>&& r)
{
	shader->Use();
	glm::mat4 tmpmodel;
	for (size_t i = 0; i<r.size(); ++i)
	{
		tmpmodel = glm::rotate(tmpmodel, glm::radians(r[i].radians), r[i].axis);
	}
	if (eject_timer <= system_time)
	{
		tmpmodel = glm::scale(tmpmodel, size);
		shader->changeMat4("model", glm::translate(translate, trans)*tmpmodel*rotate);
	}
	else
	{
		GLfloat offset = this->ejection();
		trans.y += offset / 2;
		tmpmodel = glm::scale(tmpmodel, glm::vec3(size.x, size.y + offset, size.z));
		shader->changeMat4("model", glm::translate(translate, trans)*tmpmodel*rotate);
	}
	shader->changeMat4("view", *view);
	shader->changeMat4("projection", projection);
	shader->change1f("time", this->time);
	shader->change1f("transparency", this->transparency);
	this->explode();
	object->Draw(*shader);
}
void Object::draw(GLfloat borningtime)
{
	shader->Use();
	GLfloat percent = (BORNING_TIME - borningtime) / 2 / BORNING_TIME;
	shader->change1f("time", glm::radians(percent * 360.0f));
	shader->change1f("transparency", 1 - 2 * percent);
	shader->changeMat4("model", translate*glm::scale(rotate, size));
	shader->changeMat4("view", *view);
	shader->changeMat4("projection", projection);
	this->explode();
	object->Draw(*shader);
}
void Object::explode()
{
	if (system_time < timer)
	{
		GLfloat time = EXPLODE_TIME + system_time - timer;
		shader->change1f("time", glm::radians(time / 2 / EXPLODE_TIME * 360.0f));
		shader->change1f("transparency", getPercent(time,EXPLODE_TIME));
	}
}
GLfloat Object::compress(GLfloat time,glm::vec3 moving)
{	
	shader->Use();
	GLfloat move = time * this->compressunit;
	shader->changeMat4("model", glm::translate(translate,moving+ glm::vec3(0.0f, -move/2, 0.0f))*glm::scale(rotate, glm::vec3(this->size.x, this->size.y - move, this->size.z)));
	shader->changeMat4("view", *view);
	shader->changeMat4("projection", projection);
	//this->explode();
	object->Draw(*shader);
	return move;
}
GLfloat Object::ejection()
{
	if (system_time < eject_timer)
	{
		GLfloat time = EJECTION_TIME - eject_timer + system_time;
		return eject_func(time);
	}
	else
	{
		return 0.0f;
	}
}
glm::vec3 raw_straight(GLfloat time, GLfloat horizon_radians, GLfloat vertical_radians,GLfloat length)
{
	GLfloat len = length * formatSin(time);
	return glm::vec3(len*cos(horizon_radians)*cos(vertical_radians),len*sin(vertical_radians),len*cos(vertical_radians)*sin(horizon_radians));
}
glm::vec3 raw_circle(GLfloat time, GLfloat horizon_radians, GLfloat vertical_radians, GLfloat length)
{
	GLfloat x = length * cos(glm::radians(time)*90.0f);
	GLfloat y = length * sin(glm::radians(time)*90.0f);
	GLfloat z = y * cos(vertical_radians);
	y = y * sin(vertical_radians);
	return glm::vec3(x*cos(horizon_radians)+z*sin(horizon_radians), y, x*sin(horizon_radians) + z * cos(horizon_radians));
}
glm::vec3 raw_null(GLfloat time, GLfloat horizon_radians, GLfloat vertical_radians, GLfloat length)
{
	return glm::vec3(0.0f);
}
glm::vec3 raw_parabola(GLfloat time, GLfloat horizon_radians, GLfloat vertical_radians, GLfloat length)
{
	GLfloat x = float(sqrt(length)) * sin(glm::radians(time)*90.0f);
	GLfloat y = x * x - length;
	return glm::vec3(x*cos(horizon_radians), y, x*sin(horizon_radians));
}
vector<rawmovingtrail> ObjectList::rawfunc_list({ raw_straight,raw_circle,raw_null,raw_parabola });
//vector<rawmovingtrail> ObjectList::rawfunc_list({raw_null});
ObjectList::MovingResult ObjectList::judgePosition(GLfloat time,glm::vec3 position, GLfloat radius)
{
	for (size_t index = 0; index < this->size; ++index)
	{
		Object& o = this->object_list[index];
		switch (o.object_type)
		{
			case Object::ModelType::Normal:
			{
				glm::vec3 cube = o.translate[3];
				cube += this->func_list[index](time);
				ObjectList::MovingResult result = getAABB(position, radius, { { cube.x - o.size.x / 2,cube.y - o.size.y / 2 ,cube.z - o.size.z / 2 },{ cube.x + o.size.x / 2,cube.y + o.size.y / 2 ,cube.z + o.size.z / 2 } });
				if (result != ObjectList::MovingResult::fly)
				{
					if (result == ObjectList::MovingResult::land)
					{
						this->land_object = o;
						glm::vec3 offset = cube - position;
						offset.y = 0.0f;
						this->land_object.reset(offset, o.size);
						this->land_func = this->func_list[index];
					}
					return result;
				}
				break;
			}
		}
	}
	return ObjectList::MovingResult::fly;
}
void ObjectList::generatePlatform(GLuint size)
{
	if (size > MAX_OBJECT)
	{
		size = MAX_OBJECT;
	}
	this->size = size;
	generatePoint(size, this->buf_list);
	for (GLuint i = 0; i < size; ++i)
	{
		GLuint template_index = rand() % this->template_list.size();
		GLuint func_index = rand() % ObjectList::rawfunc_list.size();
		this->object_list[i] = this->template_list[template_index];
		this->object_list[i].reset(this->buf_list[i], generateSize());
		this->func_list[i] = std::bind(ObjectList::rawfunc_list[func_index], std::placeholders::_1, getRadians(), getRadians(), rand() / (double)(RAND_MAX)*TRAIL_RANGE + MIN_TRAIL);
		//cout << "template_index:" << template_index << "\tfunc_index:" << func_index << "\nposition:" << this->buf_list[i].x << "," << this->buf_list[i].y << "," << this->buf_list[i].z << endl;
	}
}
void ObjectList::draw()
{
	for (GLuint i = 0; i < size; ++i)
	{
		this->object_list[i].draw(this->func_list[i](system_time));
	}
}
void ObjectList::draw(GLfloat time)
{
	for (GLuint i = 0; i < size; ++i)
	{
		this->object_list[i].draw(time);
	}
}
ObjectList::MovingResult getAABB(glm::vec3 sphere_center, GLfloat radius, CubeEquation&& cube)
{
	GLfloat x = getNearest(sphere_center.x, cube.min.x, cube.max.x);
	GLfloat y = getNearest(sphere_center.y, cube.min.y, cube.max.y);
	GLfloat z = getNearest(sphere_center.z, cube.min.z, cube.max.z);
	GLdouble result = pow(x, 2) + pow(y, 2) + pow(z, 2);
	if (result <= pow(radius, 2))
	{
		//cout << sphere_center.x << ";" << sphere_center.y << ";" << sphere_center.z << endl;
		//cout << cube.min.x << ";" << cube.min.y << ";" << cube.min.z << endl;
		//cout << cube.max.x << ";" << cube.max.y << ";" << cube.max.z << endl;
		//cout << x << ";" << y << ";" << z << endl;
		if (x == 0 && z == 0 && sphere_center.y >= cube.max.y)
		{
			return ObjectList::MovingResult::land;
		}
		else
			return ObjectList::MovingResult::collision;
	}
	return ObjectList::MovingResult::fly;
}
void generatePoint(GLuint size, vector<glm::vec3>& buf)
{
	glm::vec3 point;
	srand(time(NULL) + rand());
	GLuint i = 0;
	bool discard = false;
	while (i < size)
	{
		discard = false;
		do {
			GLfloat horizon = rand() / (double)(RAND_MAX)*MAX_HORIZON;
			GLfloat radians = glm::radians(rand() / (double)(RAND_MAX)*360.0f);
			GLfloat y = rand() / (double)(RAND_MAX) * 2.0f*MAX_VERTICAL + 50;
			point = glm::vec3(horizon*sin(radians), y, horizon*cos(radians));
		} while (!validPoint(point));
		for (GLuint j = 0; j < i; ++j)
		{
			if (isnear(buf[j], point))
			{
				discard = true;
			}
		}
		if (!discard)
		{
			buf[i] = point;
			++i;
		}
	}
}
glm::vec3 generateSize()
{
	GLfloat length = MIN_LENGTH + rand() / (double)(RAND_MAX)*LENGTH_RANGE;
	GLfloat width = MIN_WIDTH + rand() / (double)(RAND_MAX)*WIDTH_RANGE;
	GLfloat height = MIN_HEIGHT + rand() / (double)(RAND_MAX)*HEIGHT_RANGE;
	return glm::vec3(length, height, width);
}
GLfloat getRadians()
{
	return glm::radians(rand() / (double)(RAND_MAX)*360.0f);
}