#pragma once
#ifndef BALL_h
#define BALL_H
#include "GameObject.h"

class Ball :
	public GameObject
{
public:
	float dx = 0, dy = 0, dz = 0;
	float radius;
	int powerUpIndex = -1;
	bool isThrown = false;
	bool returned = false;
	Ball(float radius = 0, int health = 0, int maxHealth = 0);
	~Ball();
	string tag();
	void move();
	void setVelocity(float dx, float dy, float dz);
	void reset();

	void draw(GLint modelLoc, GLint colorLoc);

};

#endif