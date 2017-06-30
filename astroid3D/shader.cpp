//
//  shader.cpp
//  3d_Astroid
//

#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
using namespace std;

#include <stdlib.h>
#include <string.h>

#include <GL/glew.h>

#include "shader.hpp"

std::string readFile(const char *path) {
	std::string result;
	std::ifstream stream(path, std::ios::in);
	if (stream.is_open()) {
		std::string Line = "";
		while (getline(stream, Line))
			result += "\n" + Line;
		stream.close();
	}
	else {
		printf("%s. Check the path\n", path);
		getchar();
	}

	return result;
}

void compileShader(GLuint shaderID, std::string src) {
	int InfoLogLength;
	GLint result=GL_FALSE;

	char const *charSrc = src.c_str();

	glShaderSource(shaderID, 1, &charSrc, NULL);
	glCompileShader(shaderID);

	// check shader
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &result);
	glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(shaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}
}

/*
Load shaders including  geometry shader
*/
GLuint LoadShadersWG(const char *vertPath, const char *geomPath, const char *fragPath, GLchar **varyings, GLsizei varNum) {
	
	//create shaders
	GLuint vertID = glCreateShader(GL_VERTEX_SHADER);
	GLuint geomID = glCreateShader(GL_GEOMETRY_SHADER);
	GLuint fragID = glCreateShader(GL_FRAGMENT_SHADER);

	GLuint progID = glCreateProgram();

	//load shader src code
	/*std::string vertCode = readFile(vertPath);
	std::string geomCode = readFile(geomPath);
	std::string fragCode = readFile(fragPath);

	//compile the shaders
	std::cout << "Compiling shader: " << vertPath;
	compileShader(vertID, vertCode);

	std::cout << "Compiling shader: " << geomPath;
	compileShader(geomID, geomCode);

	std::cout << "Compiling shader: " << fragPath;
	compileShader(fragID, fragCode);*/
	if (vertPath != NULL) {
		std::string vertCode = readFile(vertPath);

		std::cout << "Compiling shader: " << vertPath << "\n";
		compileShader(vertID, vertCode);

		glAttachShader(progID, vertID);
	}

	if (geomPath != NULL) {
		std::string geomCode = readFile(geomPath);

		std::cout << "Compiling shader: " << geomPath << "\n";
		compileShader(geomID, geomCode);

		glAttachShader(progID, geomID);
	}

	if (fragPath != NULL) {
		std::string fragCode = readFile(fragPath);

		std::cout << "Compiling shader: " << fragPath << "\n";
		compileShader(fragID, fragCode);

		glAttachShader(progID, fragID);
	}

	// Link the program
	printf("Linking program\n");
	/*GLuint progID = glCreateProgram();
	glAttachShader(progID, vertID);
	glAttachShader(progID, geomID);
	glAttachShader(progID, fragID);*/

	if (varyings != NULL) {
		glTransformFeedbackVaryings(progID, varNum, varyings, GL_INTERLEAVED_ATTRIBS);
	}

	glLinkProgram(progID);

	// Check the program
	GLint result=GL_FALSE;
	int infoLogLength;
	glGetProgramiv(progID, GL_LINK_STATUS, &result);
	glGetProgramiv(progID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if (infoLogLength > 0) {
		std::vector<char> ProgramErrorMessage(infoLogLength + 1);
		glGetProgramInfoLog(progID, infoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}

	//dispose of shaders
	if (vertPath != NULL) {
		glDetachShader(progID, vertID);
		glDeleteShader(vertID);
	}

	if (geomPath != NULL) {
		glDetachShader(progID, geomID);
		glDeleteShader(geomID);
	}

	if (fragPath != NULL) {
		glDetachShader(progID, fragID);
		glDeleteShader(fragID);
	}

	return progID;
}

GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path){
    
    // Create the shaders
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
    
    // Read the Vertex Shader code from the file
    std::string VertexShaderCode=readFile(vertex_file_path);
    
    // Read the Fragment Shader code from the file
    std::string FragmentShaderCode=readFile(fragment_file_path);
    
    // Compile Vertex Shader
    printf("Compiling shader : %s\n", vertex_file_path);
	compileShader(VertexShaderID, VertexShaderCode);
    
    // Compile Fragment Shader
    printf("Compiling shader : %s\n", fragment_file_path);
	compileShader(FragmentShaderID, FragmentShaderCode);
    
    // Link the program
    printf("Linking program\n");
    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID, VertexShaderID);
    glAttachShader(ProgramID, FragmentShaderID);
    glLinkProgram(ProgramID);
    
    // Check the program
	int InfoLogLength;
	GLint Result = GL_FALSE;
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if ( InfoLogLength > 0 ){
        std::vector<char> ProgramErrorMessage(InfoLogLength+1);
        glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
        printf("%s\n", &ProgramErrorMessage[0]);
    }
    
    glDetachShader(ProgramID, VertexShaderID);
    glDetachShader(ProgramID, FragmentShaderID);
    
    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);
    
    return ProgramID;
}


