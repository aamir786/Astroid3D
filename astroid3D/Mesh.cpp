#include "Mesh.hpp"
#include "tangentspace.hpp"

Mesh::Mesh(vector<vec3> vertices, vector<vec2> uvs, vector<vec3> normals, GLuint Dtexture, GLuint Ntexture, GLuint Stexture) {
	this->vertices = vertices;
	this->uvs = uvs;
	this->normals = normals;
	this->Dtexture = Dtexture;
	this->Ntexture = Ntexture;
	this->Stexture = Stexture;

	std::vector<glm::vec3> tangents;
	std::vector<glm::vec3> bitangents;
	computeTangentBasis(vertices, uvs, normals, tangents, bitangents);

	this->tangents = tangents;
	this->bitangents = bitangents;

	//calculate the radius
	float radSquared = 0;
	for (int i = 0; i < vertices.size(); i++) {
		vec3 v = vertices[i];
		float current = v.x*v.x + v.y*v.y + v.z*v.z;
		if (current > radSquared) {
			radSquared = current;
		}
	}
	this->radius = sqrt(radSquared);

	this->init();
}
//extern short nFunction;
void Mesh::init() {

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	//create vertex VBO
	glGenBuffers(1, &vVbo);
	glBindBuffer(GL_ARRAY_BUFFER, vVbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(vec3), &vertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	//create uv VBO
	glGenBuffers(1, &uvVbo);
	glBindBuffer(GL_ARRAY_BUFFER, uvVbo);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(vec2), &uvs[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

	//create normal VBO
	glGenBuffers(1, &nVbo);
	glBindBuffer(GL_ARRAY_BUFFER, nVbo);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(vec3), &normals[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);

	//create tangent VBO
	glGenBuffers(1, &tVbo);
	glBindBuffer(GL_ARRAY_BUFFER, tVbo);
	glBufferData(GL_ARRAY_BUFFER, tangents.size() * sizeof(vec3), &tangents[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, 0);

	//create bitangent VBO
	glGenBuffers(1, &btVbo);
	glBindBuffer(GL_ARRAY_BUFFER, btVbo);
	glBufferData(GL_ARRAY_BUFFER, bitangents.size() * sizeof(vec3), &bitangents[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

Mesh::~Mesh() {
	glDeleteBuffers(1, &vVbo);
	glDeleteBuffers(1, &uvVbo);
	glDeleteBuffers(1, &nVbo);
	glDeleteBuffers(1, &tVbo);
	glDeleteBuffers(1, &btVbo);
	glDeleteVertexArrays(1, &vao);
}

//references: https://www.khronos.org/opengl/wiki/Tutorial2:_VAOs,_VBOs,_Vertex_and_Fragment_Shaders_(C_/_SDL)
