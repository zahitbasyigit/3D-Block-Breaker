#include "Cube.h"

Cube::Cube(point4 bottomLeftCorner, float size, int health, int maxHealth) : GameObject(health,maxHealth) {
	this->bottomLeftCorner = bottomLeftCorner;
	this->size = size;
	this->health = health;
	this->cubeVertexIndex = 0;
	this->lineVertexIndex = 0;
	this->destroyOnImpact = true;
	this->collisionType = cubeCollision;
	this->color = color4(0.0f,1.0f,0.0f,1.0f);

	//Create a cube with 6 faces, similar to assignments.

	makeFace(0, 1, 3, 2, vec3(0.0, 0.0, 1.0)); //FRONT
	makeFace(1, 5, 7, 3, vec3(1.0, 0.0, 0.0)); //RIGHT
	makeFace(0, 4, 6, 2, vec3(-1.0, 0.0, 0.0)); //LEFT
	makeFace(4, 5, 7, 6, vec3(0.0, 0.0, -1.0)); //BACK
	makeFace(0, 1, 5, 4, vec3(0.0, -1.0, 0.0)); //BOTTOM
	makeFace(2, 3, 7, 6, vec3(0.0, 1.0, 0.0)); //TOP

	for (int i = 0; i < numberOfVerticesPerCube; i++) {
		this->vertices.push_back(cubePoints[i]);
		this->normals.push_back(cubeNormals[i]);
	}

	for (int j = 0; j < numberOfVerticesOfLinesPerCube; j++) {
		this->vertices.push_back(linePoints[j]);
	}

}


Cube::~Cube()
{
}


void Cube::makeFace(int i1, int i2, int i3, int i4, vec3 normal) {

	float x = bottomLeftCorner.x;
	float y = bottomLeftCorner.y;
	float z = bottomLeftCorner.z;

	point4 vertices[8] = {
		point4(x,y,z,1.0f),
		point4(x + size,y,z,1.0f),
		point4(x,y + size,z,1.0f),
		point4(x + size,y + size,z,1.0f),
		point4(x,y,z - size,1.0f),
		point4(x + size,y,z - size,1.0f),
		point4(x,y + size,z - size,1.0f),
		point4(x + size,y + size,z - size,1.0f),
	};
	
	//set the cubes collision bounds in terms of x,y,z.
	xCollisionLeftBound = vertices[0].x;
	xCollisionRightBound = vertices[1].x;
	yCollisionTopBound = vertices[2].y;
	yCollisionBottomBound = vertices[0].y;
	zCollisionBackBound = vertices[0].z;
	zCollisionFrontBound = vertices[4].z;

	cubeNormals[cubeVertexIndex] = normal;
	cubePoints[cubeVertexIndex++] = vertices[i1];
	cubeNormals[cubeVertexIndex] = normal;
	cubePoints[cubeVertexIndex++] = vertices[i2];
	cubeNormals[cubeVertexIndex] = normal;
	cubePoints[cubeVertexIndex++] = vertices[i3];

	cubeNormals[cubeVertexIndex] = normal;
	cubePoints[cubeVertexIndex++] = vertices[i3];
	cubeNormals[cubeVertexIndex] = normal;
	cubePoints[cubeVertexIndex++] = vertices[i4];
	cubeNormals[cubeVertexIndex] = normal;
	cubePoints[cubeVertexIndex++] = vertices[i1];

	linePoints[lineVertexIndex++] = vertices[i1];
	linePoints[lineVertexIndex++] = vertices[i2];
	linePoints[lineVertexIndex++] = vertices[i2];
	linePoints[lineVertexIndex++] = vertices[i3];
	linePoints[lineVertexIndex++] = vertices[i3];
	linePoints[lineVertexIndex++] = vertices[i4];
	linePoints[lineVertexIndex++] = vertices[i4];
	linePoints[lineVertexIndex++] = vertices[i1];

	

}

string Cube::tag()
{
	return "Cube";
}

void Cube::draw(GLint modelLoc, GLint colorLoc) {
	if (this->isVisible) {
		glBindVertexArray(this->vao);
		glUniformMatrix4fv(modelLoc, 1, GL_TRUE, this->model);
		glUniform4fv(colorLoc, 1, this->color);
		glDrawArrays(GL_TRIANGLES, 0, this->numberOfVerticesPerCube);
		glLineWidth(2);
		glDrawArrays(GL_LINES, this->numberOfVerticesPerCube, this->numberOfVerticesOfLinesPerCube);
	
	}
}