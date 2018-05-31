#pragma once
#define GYROSCOPE_STATE 0x1
#define INIT_TIMES 10
#define DISTANCE_VALUE 1.0f
#define ANGLE_RANGE 250.0f
#define MEASURE_RANGE 32768.0f
typedef int devicestate;
struct EmbeddedDevice
{
	bool gyroscope_state;
	float yaw;
	float pitch;
};
inline float standardlizeAcc(float acc)
{
	float result = acc / MEASURE_RANGE * ANGLE_RANGE;
	return result < DISTANCE_VALUE ? 0.0f : result;
}