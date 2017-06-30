#ifndef PARTICLE_HPP
#define PARTICLE_HPP

#include <glm/glm.hpp>
using namespace glm;
#include <GL/glew.h>

/*class Particle {
public:
	vec3 pos;
	vec3 v;
	float age;
	float timeToLive;
};*/

struct Particle {
	vec3 pos;
	vec3 vel;
	float time;
};
#endif