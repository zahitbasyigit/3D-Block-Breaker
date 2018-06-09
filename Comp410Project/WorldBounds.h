#pragma once

#ifndef WORLD_BOUNDS_h
#define WORLD_BOUNDS_H

#include "GameObject.h"
#include "WorldSide.h"

class WorldBounds :
	public GameObject
{
public:
	static const int worldSizeByCubesX = 6;
	static const int worldSizeByCubesY = 4;
	static const int worldSizeByCubesZ = 12;
	static const int numberOfWorldSideBounds = 5;
	static const int vertexPerWorldSideBound = 6;
	static const int lineVertexPerWorldSideBound = 8;
	static const float cubeSize;

	WorldBounds(int health = 0, int maxHealth = 0);
	~WorldBounds();

	string tag();
	void draw(GLint modelLoc, GLint colorLoc);
};

#endif