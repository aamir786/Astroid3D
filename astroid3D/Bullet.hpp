#ifndef BULLET_HPP
#define BULLET_HPP
#include "GameObject.hpp"
#include "Mesh.hpp"

class Bullet : public GameObject {
public:
	float timeToLive;
	float age;
	float damage;

	Bullet(float timeToLive, float damage, Mesh *mesh);
	virtual void update(float time);
	virtual bool isAlive();
};

#endif
