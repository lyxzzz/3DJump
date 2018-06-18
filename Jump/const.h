#pragma once
#define VERSION 2
#define CURSOR_WIDTH 1
#define CURSOR_HEIGHT 1
#define WIDTH 1600
#define HEIGHT 900
#define RADIUS 5.0f
#define XRAN 360.0f
#define YRAN 75.0f
#define PERSPECTIVE_RADIANS glm::radians(45.0f)
#define MAX_POINT_BUFFER 1024
#define FLOAT_BIT 4
#define CURSOR_SIZE 0.3f

#define MAX_INTENSITY 2.0f
#define EJECTION_TIME 0.35f
#define PERIOD_TIMES 1.75f

#define ZMIN 0.3f
#define ZMAX 4.0f
#define EXPLODE_TIME 3.0f

#define MUPPET_HEIGHT 0.002f
#define MUPPET_WIDTH 0.002f
#define MUPPET_LENGTH 0.002f
#define MUPPET_POSITION glm::vec3(0.0f,-1.8f,0.0f)
#define MUPPET_RADIANS -180.0f

#define CAT_HEIGHT 0.04f
#define CAT_WIDTH 0.04f
#define CAT_LENGTH 0.04f
#define CAT_HEIGHT 0.04f
#define CAT_POSITION glm::vec3(0.0f,-1.8f,0.0f)
#define CAT_RADIANS -90.0f

#define SPHERE_HEIGHT 0.0017157f
#define SPHERE_WIDTH 0.0017157f
#define SPHERE_LENGTH 0.001
#define SHIELD_POSITION glm::vec3(0.0f,-1.3f,0.0f)
#define SHIELD_RADIANS 0.0f

#define OBJECT_OFFSET glm::vec3(0.0f,-2.3f,0.0f)

#define MAX_FLYING_TIME 5.0f
#define MAX_HORIZON 80.0f
#define MAX_VERTICAL -40.0f
#define HORIZON_SPEED 16.0f
#define VERTICAL_ACCELERATION 19.2f
#define MAX_VERTICAL_SPEED 40.0f
#define SPEED_POWER 10.0f
#define SHIELD_CONSUME 0.18f

#define BORNING_TIME 3.0f
#define GENERATING_TIME 3.0f
#define FLYING_ANGULAR -180.0f

#define MAX_OBJECT 10

#define MIN_DISTANCE 100.0f

#define WIDTH_RANGE 6.0f
#define MIN_WIDTH 2.0f
#define LENGTH_RANGE 6.0f
#define MIN_LENGTH 2.0f
#define HEIGHT_RANGE 2.0f
#define MIN_HEIGHT 1.0f

#define TRAIL_RANGE 5.0f
#define MIN_TRAIL 3.0f

#define FLYING_RESISTENCE 75.0f

#define SPHERE_RADIUS 0.5f
#define PLAYER_OFFSET glm::vec3(0.0f,-1.8f,0.0f)

#define NANOSUIT_HEIGHT 0.06f
#define NANOSUIT_WIDTH 0.3f
#define NANOSUIT_LENGTH 0.3f
#define NANOSUIT_POSITION glm::vec3(0.0f,-1.8f,0.0f)
#define NANOSUIT_RADIANS 180.0f

#define CIRCLE_HEIGHT 0.0075f
#define CIRCLE_WIDTH 0.0075f
#define CIRCLE_LENGTH 0.0075f

#define VIEW_INIT glm::vec3(0.0f,0.0f,0.0f)
enum ModelState {
	Stillness,
	Accumlating,
	Lauching,
	Flying,
	Moving,
	Borning,
	Generating,
	Reborn,
	Pause
};