#include "Object.h"


Object::Object()
{
	this->vectorIndex = -1;
	this->model = mat4(1.0f);

}


Object::~Object()
{

}

int Object::getVectorIndex() {
	return this->vectorIndex;
}

void Object::setVectorIndex(int vectorIndex) {
	this->vectorIndex = vectorIndex;
}

vector<point4> Object::getVertices()
{
	return this->vertices;
}

vector<vec3> Object::getNormals()
{
	return this->normals;
}

mat4 Object::getModel()
{
	return this->model;
}

void Object::displaceModel(float dx, float dy, float dz)
{
	//Displace a model by dx,dy,dz and update its model matrix.
	this->x += dx;
	this->y += dy;
	this->z += dz;
	updateMatrices();
}

void Object::scaleModel(float scale) {
	//Scale a model by scale and update its model matrix.
	this->scale = scale;
	updateMatrices();
}

void Object::rotateModel(vec3 point, float dx, float dy, float dz)
{
	//Rotate a model by dx,dy,dz in terms of angles around a point and update its model matrix.
	this->rotateAround = point;
	this->xRotation = this->xRotation + dx;
	this->yRotation = this->yRotation + dy;
	this->zRotation = this->zRotation + dz;
	updateMatrices();
}

void Object::updateMatrices()
{

	//Set the model matrix with correct order of transformations.
	this->model = Translate(rotateAround.x,rotateAround.y,rotateAround.z) * RotateZ(zRotation) * RotateY(yRotation) * RotateX(xRotation) * Translate(-rotateAround.x,-rotateAround.y,-rotateAround.z) *
		Translate(this->x, this->y, this->z) * Scale(this->scale, this->scale, this->scale);
}

void Object::populateVertices(vector<point4> vertices, vector<vec3> normals)
{
	for (int i = 0; i < vertices.size(); i++) {
		this->vertices.push_back(vertices.at(i));
	}
	for (int i = 0; i < normals.size(); i++) {
		this->normals.push_back(normals.at(i));
	}
}






