#include "Bullet.hpp"

Bullet::Bullet(float timeToLive, float damage, Mesh *mesh) : GameObject(mesh) {
	this->timeToLive = timeToLive;
	this->age = 0;
	this->damage = damage;
}

void Bullet::update(float time) {
	GameObject::update(time);
	age += time;
}

bool Bullet::isAlive() {
	return age < timeToLive;
}
