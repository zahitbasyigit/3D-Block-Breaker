#pragma once
#include "Object.h"
class Crosshair :
	public Object
{
public:
	mat4 projection;
	Crosshair();
	~Crosshair();
	void draw(GLint modelLoc, GLint colorLoc);
	string tag();
};

