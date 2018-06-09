#pragma once
#ifndef OBJECT_HOLDER_h
#define OBJECT_HOLDER_H

#include "WorldBounds.h"
#include "Cube.h"
#include "Ball.h"
#include "Crosshair.h"
#include <vector>

using namespace std;

class ObjectHolder
{
private:
	vector<Object*> objects;
	vector<GameObject*> collidableGameObjects;
	vector<Ball*> powerUpSpheres;
public:
	mat4 perspectiveProjection, orthogonalProjection, view;
	bool lightAsPoint = false;
	int currentTurnCount;
	int currentTurnBallCount;
	int ballCount;
	GLuint program;
	vec4 shootingVector = vec4(0.0f, 0.0f, -0.5f, 0.0f);
	float xRotationByDegrees = 0, yRotationByDegrees = 0;
	vector<Ball*> helperBalls;
	vector<Ball*> shootingBalls;
	Ball* startingBall;
	Ball* lightBall;

	WorldBounds* worldBounds;
	bool newTurnAnimation = false;
	float newTurnTotalDisplacement = 0;
	bool gameOver = false;
	bool allowShoot = false;
	ObjectHolder();
	~ObjectHolder();
	void addObject(Object* object);
	void addCollidableGameObject(GameObject* gameObject);
	void addPowerUpSphere(Ball* ball);
	void removeObject(Object* object, bool deletePointer);
	void removeCollidableGameObject(GameObject* object, bool deletePointer);
	void removePowerUpShere(Ball* object, bool deletePointer);
	void drawObjects(GLint modelLoc, GLint viewLoc, GLint projectionLoc, GLint colorLoc);
	void moveBall(Ball* ball);
	void createRandomBox(float leftBound, float rightBound, float bottomBound, float topBound, float frontBound, float backBound, float boxSize);
	float randomFloat(float min, float max);
	void generateHelperBalls(vec4 vector);
	void rotateHelperBalls(float dx, float dy);
	void translateHelperBallsToPosition(float x, float y);
	void showHelperBalls(bool show);
	Ball* generateBall(float radius, vec3 position);
	void generateSquaresOnNewTurn();
	vector<point4> generateRandomBottomLeftCorners(float cubeSize);
	void bringCubesCloser(float dz);
	void newTurn();
	void resetGame();
	void update();
	float distanceBtwPoints(vec3 p1, vec3 p2);
	void addNewShootingBalls();
	void moveStartingBall(float dx, float dy);
	void setLightAsPoint(bool pointLight);
	void updateLight();
};

#endif