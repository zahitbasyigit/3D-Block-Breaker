#include "Ball.h"



Ball::Ball(float radius, int health, int maxHealth) : GameObject(health,maxHealth) {
	this->radius = radius;
	this->collisionType = sphereCollision;

}


Ball::~Ball()
{
}

string Ball::tag()
{
	return "Ball";
}

void Ball::move()
{
	displaceModel(dx, dy, dz);
}

void Ball::setVelocity(float dx, float dy, float dz)
{
	this->dx = dx;
	this->dy = dy;
	this->dz = dz;
}

void Ball::reset()
{
	//set the balls position to z = 0 plane. velocity to 0.
	this->z = 0;
	this->dx = 0;
	this->dy = 0;
	this->dz = 0;
}
void Ball::draw(GLint modelLoc, GLint colorLoc) {
	if (this->isVisible) {
		glBindVertexArray(this->vao);
		glUniformMatrix4fv(modelLoc, 1, GL_TRUE, this->model);
		glUniform4fv(colorLoc, 1, this->color);
		glDrawArrays(GL_TRIANGLES, 0, this->vertices.size());
	}
}

