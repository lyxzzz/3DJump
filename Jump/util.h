#ifndef MYUTIL_H
#define MYUTIL_H
#include<math.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include"const.h"
#define SPEED 45
#include<iostream>
using namespace std;
inline float getPercent(float time,float range)
{
	return 1 - sin(glm::radians((time / range) * 90));
}
inline void getAccumlatingTime(float time,glm::vec3& size)
{
	if (time > MAX_INTENSITY)
	{
		time = MAX_INTENSITY;
	}
	size.y *= (1 - (time / MAX_INTENSITY / 2.0f));
}
inline float formatSin(float time)
{
	return (1 - cos(glm::radians(time) * 90)) / 2;
}
inline bool validPoint(glm::vec3 point)
{
	float horizon = sqrt(pow(point.x, 2) + pow(point.z, 2));
	if (horizon <= 8.0f)
	{
		return false;
	}
	float time = horizon / HORIZON_SPEED;
	float vertical_speed = (point.y + 0.5*VERTICAL_ACCELERATION*time*time) / time;
	//cout << "vertical_speed£º" << vertical_speed << endl;
	if (vertical_speed <= 0 || vertical_speed > MAX_VERTICAL_SPEED)
	{
		return false;
	}
	if (vertical_speed - time * VERTICAL_ACCELERATION > 0)
	{
		return false;
	}
	float accmulating_time = sqrt(vertical_speed / SPEED_POWER);
	//cout << "accmulating_time£º" << accmulating_time << endl;
	if (accmulating_time / MAX_INTENSITY / SHIELD_CONSUME < time)
	{
		return false;
	}
	//cout << "true" << endl;
	return true;
}
inline bool isnear(glm::vec3 point1, glm::vec3 point2)
{
	glm::vec3 tmp = point1 - point2;
	float result = pow(tmp.x, 2) + pow(tmp.y, 2) + pow(tmp.z, 2);
	return result < MIN_DISTANCE;
}
inline float distance(float v1, float v2)
{
	return v1 > v2 ? v1 - v2 : v2 - v1;
}
#endif // MYTIME
