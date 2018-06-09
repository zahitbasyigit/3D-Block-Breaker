#include "GameObject.h"
#include <iostream>


GameObject::GameObject(int health, int maxHealth) : Object()
{
	this->health = health;
	this->maxHealth = maxHealth;
	this->perspectiveProjection = true;
}


GameObject::~GameObject()
{

}
