#ifndef GAMEOBJECT_HPP
#define GAMEOBJECT_HPP

#include "Mesh.hpp"
#include <glm/glm.hpp>
using namespace glm;
#include <glm/gtc/quaternion.hpp>

/*
	Encapsulates game object properties
*/
class GameObject {
public:
	Mesh *mesh;
	vec3 position; //TODO: add getters/setters for xyz
	quat rotation; //rotation in quaternions
	float scale;

	vec3 velocity;
	quat rotVel; //rotation velocity in quaternions

	float hp;

	GameObject(Mesh *mesh);
	mat4x4 getModelMatrix();
	virtual void update(float time);
	void addRotation(vec3 axis, float angle);
	virtual bool isAlive();
	float getRadius();
	bool collides(GameObject *other);
};

#endif
