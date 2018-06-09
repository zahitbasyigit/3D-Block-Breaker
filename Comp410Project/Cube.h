#pragma once
#ifndef CUBE_h
#define CUBE_H
#include "GameObject.h"


class Cube :
	public GameObject
{
private:
	static const int numberOfVerticesOfLinesPerCube = 48; //6 Sides * 4 lines * 2 VertexPerLine
	static const int numberOfVerticesPerCube = 36; //6(Side)*2(Triangle)*3(VertexPerTriangle) = 36

	point4 linePoints[numberOfVerticesOfLinesPerCube];

	point4 cubePoints[numberOfVerticesPerCube];
	vec3 cubeNormals[numberOfVerticesPerCube];
	
	point4 bottomLeftCorner;

	float size;
	int cubeVertexIndex;
	int lineVertexIndex;

public:
	Cube(point4 bottomLeftCorner = point4(0), float size = 0, int health = 0, int maxHealth = 0);
	~Cube();
	void makeFace(int i1, int i2, int i3, int i4, vec3 normal);
	string tag();
	void draw(GLint modelLoc, GLint colorLoc);
};

#endif