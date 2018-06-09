#pragma once
#ifndef GAME_OBJECT_h
#define GAME_OBJECT_H

#include "Object.h"

class GameObject :
	public Object
{
public:
	enum {cubeCollision = 0, sphereCollision = 1, worldBoundsCollision = 2};
	int collisionType;
	int numberOfTurnsExisted = 0;
	int collidableVectorIndex = -1;
	int health;
	int maxHealth;
	float xCollisionLeftBound, xCollisionRightBound, yCollisionBottomBound, yCollisionTopBound, zCollisionFrontBound, zCollisionBackBound;
	bool destroyOnImpact = false;
	GameObject(int health, int maxHealth);
	~GameObject();
};

#endif
