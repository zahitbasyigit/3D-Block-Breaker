#include "WorldSide.h"
#include "WorldBounds.h"


WorldSide::WorldSide()
{
}

WorldSide::WorldSide(point4 bottomLeftCorner, point4 bottomRightCorner, point4 topLeftCorner, point4 topRightCorner, vec3 normal)
{
	index = 0;
	lineIndex = 0;

	sideBoundNormals[index] = normal;
	sideBoundVertices[index++] = bottomLeftCorner;
	sideBoundNormals[index] = normal;
	sideBoundVertices[index++] = bottomRightCorner;
	sideBoundNormals[index] = normal;
	sideBoundVertices[index++] = topRightCorner;
	sideBoundNormals[index] = normal;
	sideBoundVertices[index++] = topRightCorner;
	sideBoundNormals[index] = normal;
	sideBoundVertices[index++] = topLeftCorner;
	sideBoundNormals[index] = normal;
	sideBoundVertices[index++] = bottomLeftCorner;

	sideBoundLineVertices[lineIndex++] = bottomLeftCorner;
	sideBoundLineVertices[lineIndex++] = bottomRightCorner;
	sideBoundLineVertices[lineIndex++] = bottomRightCorner;
	sideBoundLineVertices[lineIndex++] = topRightCorner;
	sideBoundLineVertices[lineIndex++] = topRightCorner;
	sideBoundLineVertices[lineIndex++] = topLeftCorner;
	sideBoundLineVertices[lineIndex++] = topLeftCorner;
	sideBoundLineVertices[lineIndex++] = bottomLeftCorner;

}


WorldSide::~WorldSide()
{
}
