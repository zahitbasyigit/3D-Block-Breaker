#pragma once
#ifndef OBJECT_h
#define OBJECT_H
#include "Angel.h"
#include <vector>
typedef Angel::vec4  color4;
typedef Angel::vec4  point4;
using namespace std;

class Object
{

private:
	int vectorIndex;
public:
	mat4 model;
	bool perspectiveProjection;
	color4 color = color4(0.0f, 0.0f, 0.0f, 1.0f);
	float x = 0, y = 0, z = 0;
	float scale = 1;
	float xRotation = 0, yRotation = 0 , zRotation = 0;
	vec3 rotateAround = vec3(0.0f);
	bool isVisible = true;
	GLuint vao;
	GLuint buffer;
	vector<point4> vertices;
	vector<vec3> normals;

	Object();
	~Object();
	
	virtual void draw(GLint modelLoc, GLint colorLoc) = 0;
	virtual string tag() = 0;

	int getVectorIndex();
	void setVectorIndex(int vectorIndex);
	vector<point4> getVertices();
	vector<vec3> getNormals();
	mat4 getModel();
	void displaceModel(float dx, float dy, float dz);
	void scaleModel(float scale);
	void rotateModel(vec3 point, float dx, float dy, float dz);
	void updateMatrices();
	void populateVertices(vector<point4> vertices, vector<vec3> normals);
};

#endif