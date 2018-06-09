#pragma once
#ifndef WORLD_SIDE_h
#define WORLD_SIDE_H
#include "WorldSide.h"
#include "WorldBounds.h"

class WorldSide
{
public:
	point4 sideBoundVertices[6];
	vec3 sideBoundNormals[6];
	point4 sideBoundLineVertices[8];
	int index;
	int lineIndex;

	WorldSide();
	WorldSide(point4 bottomLeftCorner, point4 bottomRightCorner, point4 topLeftCorner, point4 topRightCorner, vec3 normal);
	~WorldSide();
};

#endif