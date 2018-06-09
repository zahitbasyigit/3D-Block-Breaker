#include "WorldBounds.h"
#include "WorldSide.h"


const float WorldBounds::cubeSize = 2.0f;
WorldSide worldSideBounds[WorldBounds::numberOfWorldSideBounds];

WorldBounds::WorldBounds(int health, int maxHealth) : GameObject(health,maxHealth)
{
	this->destroyOnImpact = false;
	this->collisionType = worldBoundsCollision;
	this->health = health;
	point4 firstCubePosition = point4(-worldSizeByCubesX / 2 * cubeSize, -worldSizeByCubesY / 2 * cubeSize, 0.0f, 1.0f);

	float x = firstCubePosition.x;
	float y = firstCubePosition.y;
	float z = firstCubePosition.z;

	point4 rectangle3DVertices[8] = {
		point4(x,y,z,1.0f),
		point4(x + cubeSize * worldSizeByCubesX,y,z,1.0f),
		point4(x,y + cubeSize * worldSizeByCubesY,z,1.0f),
		point4(x + cubeSize * worldSizeByCubesX,y + cubeSize * worldSizeByCubesY,z,1.0f),
		point4(x,y,z - cubeSize * worldSizeByCubesZ,1.0f),
		point4(x + cubeSize * worldSizeByCubesX,y,z - cubeSize * worldSizeByCubesZ,1.0f),
		point4(x,y + cubeSize * worldSizeByCubesY,z - cubeSize * worldSizeByCubesZ,1.0f),
		point4(x + cubeSize * worldSizeByCubesX,y + cubeSize * worldSizeByCubesY,z - cubeSize * worldSizeByCubesZ,1.0f),
	};

	//Create the edges of the world and set the collision bounds in terms of x,y,z values.
	worldSideBounds[0] = WorldSide(rectangle3DVertices[0], rectangle3DVertices[4], rectangle3DVertices[2], rectangle3DVertices[6], vec3(1.0, 0.0, 0.0));//LEFT BOUND
	xCollisionLeftBound = rectangle3DVertices[0].x;

	worldSideBounds[1] = WorldSide(rectangle3DVertices[1], rectangle3DVertices[5], rectangle3DVertices[3], rectangle3DVertices[7], vec3(-1.0, 0.0, 0.0));//RIGHT BOUND
	xCollisionRightBound = rectangle3DVertices[1].x;

	worldSideBounds[2] = WorldSide(rectangle3DVertices[2], rectangle3DVertices[3], rectangle3DVertices[6], rectangle3DVertices[7], vec3(0.0, -1.0, 0.0));//TOP BOUND
	yCollisionTopBound = rectangle3DVertices[2].y;

	worldSideBounds[3] = WorldSide(rectangle3DVertices[0], rectangle3DVertices[1], rectangle3DVertices[4], rectangle3DVertices[5], vec3(0.0, 1.0, 0.0));//BOTTOM BOUND
	yCollisionBottomBound = rectangle3DVertices[0].y;

	worldSideBounds[4] = WorldSide(rectangle3DVertices[4], rectangle3DVertices[5], rectangle3DVertices[6], rectangle3DVertices[7], vec3(0.0, 0.0, 1.0));//BACK BOUND
	zCollisionBackBound = rectangle3DVertices[4].z;

	//FRONT BOUND
	zCollisionFrontBound = rectangle3DVertices[0].z;

	for (int i = 0; i < numberOfWorldSideBounds; i++) {
		for (int j = 0; j < vertexPerWorldSideBound; j++) {
			vertices.push_back(worldSideBounds[i].sideBoundVertices[j]);
			normals.push_back(worldSideBounds[i].sideBoundNormals[j]);
		}
	}

	for (int i = 0; i < numberOfWorldSideBounds; i++) {
		for (int j = 0; j < lineVertexPerWorldSideBound; j++) {
			vertices.push_back(worldSideBounds[i].sideBoundLineVertices[j]);
		}
	}


	this->color = color4(1.0f, 0.0f, 0.0f, 1.0f);
}


WorldBounds::~WorldBounds()
{
}

string WorldBounds::tag() {
	return "World";
}


void WorldBounds::draw(GLint modelLoc, GLint colorLoc) {
	if (this->isVisible) {
		glBindVertexArray(this->vao);
		glUniformMatrix4fv(modelLoc, 1, GL_TRUE, this->model);
		glUniform4fv(colorLoc, 1, this->color);
		glDrawArrays(GL_TRIANGLES, 0, numberOfWorldSideBounds * vertexPerWorldSideBound);
		glLineWidth(2);
		glDrawArrays(GL_LINES, numberOfWorldSideBounds * vertexPerWorldSideBound, numberOfWorldSideBounds * lineVertexPerWorldSideBound);

	}
}
