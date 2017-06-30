#ifndef MESH_HPP
#define MESH_HPP

#include <vector>
#include <string>

#include <glm/glm.hpp>
#include <GL/glew.h>

using std::vector;
using std::string;
using namespace glm;

struct Texture { //making it a struct in case we need some more properties
	GLuint id;
	string name; //name of the sampler uniform inside shader
};

/*
	Stores static geometry informaton for rendering
*/
class Mesh {
public:
	//PROPERTIES:
	vector<vec3> vertices;
	vector<vec2> uvs;
	vector<vec3> normals;
	GLuint texture;
	float radius;

	GLuint vao;
	GLuint vVbo; //vertex vbo
	GLuint uvVbo; //texture coordinates vbo
	GLuint nVbo; //normal vbo

	//METHODS:
	Mesh(vector<vec3> vertices, vector<vec2> uvs, vector<vec3> normals, GLuint texture);
	~Mesh();

private:
	void init();

};

#endif // !MESH_HPP
