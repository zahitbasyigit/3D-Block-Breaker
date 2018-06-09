#include "Crosshair.h"



Crosshair::Crosshair() : Object()
{

	this->perspectiveProjection = false;
	this->color = color4(1.0, 1.0, 1.0, 1.0);

	// a simple black rectangle
	point4 v1 = point4(-0.01f, -0.01f, 0.0f, 1.0f);
	point4 v2 = point4(-0.01f, 0.01f, 0.0f, 1.0f);
	point4 v3 = point4(0.01f, 0.01f, 0.0f, 1.0f);
	point4 v4 = point4(0.01f, -0.01f, 0.0f, 1.0f);

	vertices.push_back(v1);
	vertices.push_back(v2);
	vertices.push_back(v3);
	vertices.push_back(v3);
	vertices.push_back(v4);
	vertices.push_back(v1);

	normals.push_back(vec3(0.0, 0.0, 0.0));
	normals.push_back(vec3(0.0, 0.0, 0.0));
	normals.push_back(vec3(0.0, 0.0, 0.0));
	normals.push_back(vec3(0.0, 0.0, 0.0));
	normals.push_back(vec3(0.0, 0.0, 0.0));
	normals.push_back(vec3(0.0, 0.0, 0.0));


}


Crosshair::~Crosshair()
{
}

void Crosshair::draw(GLint modelLoc, GLint colorLoc) {
	if (this->isVisible) {
		glBindVertexArray(this->vao);
		glUniformMatrix4fv(modelLoc, 1, GL_TRUE, this->model);
		glUniform4fv(colorLoc, 1, this->color);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
}

string Crosshair::tag() {
	return "Crosshair";
}