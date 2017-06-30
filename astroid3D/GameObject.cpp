#include "GameObject.hpp"

#include <glm/gtx/transform.hpp>

GameObject::GameObject(Mesh *mesh) {
	//TODO: implement rotation
	this->mesh = mesh;
	this->position = vec3(0, 0, 0);
	this->velocity = vec3(0, 0, 0);
	this->rotation = quat(1, 0, 0, 0);
	this->rotVel = quat(1, 0, 0, 0);
	this->scale = 1;
}

mat4 GameObject::getModelMatrix() {
	return glm::translate(position) * mat4_cast(rotation) * glm::scale(vec3(scale, scale, scale));
}

void GameObject::update(float time) {
	position = position + time*velocity;
	rotation = mix(quat(1, 0, 0, 0), rotVel, time) * rotation;
}

void GameObject::addRotation(vec3 axis, float angle) {
	rotVel = glm::angleAxis(angle, axis) * rotVel;
}

bool GameObject::isAlive() {
	return hp>0;
}

float GameObject::getRadius() {
	return mesh->radius*scale;
}

bool GameObject::collides(GameObject *other) {
	float dx = position.x - other->position.x;
	float dy = position.y - other->position.y;
	float dz = position.z - other->position.z;
	float dist = sqrt(dx*dx + dy*dy + dz*dz);
	return dist < getRadius() + other->getRadius(); //TODO: maybe add a separate var for squared radius?
}
