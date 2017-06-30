//
//  main.cpp
//  3d_Astroid
//

#include <stdio.h>
#include <string>
#include <sstream>
#include <iostream>
#include <stdlib.h>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include "shader.hpp"
#include "texture.hpp"
#include "controls.hpp"
#include "objloader.hpp"
#include "Mesh.hpp"
#include "GameObject.hpp"
#include "Bullet.hpp"
#include "Particle.hpp"

const int MAX_PARTICLES = 10000;

GLFWwindow* window;

Mesh *rock;

GLuint programID;
GLuint blurProgramID;

GLuint framebuffer;
GLuint texColorBuffer;
GLuint depthrenderbuffer;

//ids of uniforms
GLuint modelID, viewID, projectionID, lightColorID, lightPosID, viewPosID, exponentID, specularMultID, textureID;

vector<Bullet> bullets; //list of bullets
vector<GameObject> asteroids; //list of asteroids

void renderMesh(Mesh *mesh) {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mesh->texture);
	glUniform1i(textureID, 0);
	glBindVertexArray(mesh->vao);

	glDrawArrays(GL_TRIANGLES, 0, mesh->vertices.size());

	glBindVertexArray(0);
}

void renderGameObject(GameObject *obj) {
	glUniformMatrix4fv(modelID, 1, GL_FALSE, &obj->getModelMatrix()[0][0]);
	renderMesh(obj->mesh);
}

/*
	GLFW mouse button event listener
*/
void onMouseButton(GLFWwindow* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		Bullet foo = Bullet(3, 10, rock);
		foo.position = getViewPos();
		foo.velocity = 10.0f * getViewDirection();
		foo.scale = 0.05; //temporarily, until we get bullet models
		bullets.push_back(foo);
	}
}

static std::string FormatDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, const char* msg) {
	std::stringstream stringStream;
	std::string sourceString;
	std::string typeString;
	std::string severityString;

	// The AMD variant of this extension provides a less detailed classification of the error,
	// which is why some arguments might be "Unknown".
	switch (source) {
	case GL_DEBUG_CATEGORY_API_ERROR_AMD:
	case GL_DEBUG_SOURCE_API: {
		sourceString = "API";
		break;
	}
	case GL_DEBUG_CATEGORY_APPLICATION_AMD:
	case GL_DEBUG_SOURCE_APPLICATION: {
		sourceString = "Application";
		break;
	}
	case GL_DEBUG_CATEGORY_WINDOW_SYSTEM_AMD:
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM: {
		sourceString = "Window System";
		break;
	}
	case GL_DEBUG_CATEGORY_SHADER_COMPILER_AMD:
	case GL_DEBUG_SOURCE_SHADER_COMPILER: {
		sourceString = "Shader Compiler";
		break;
	}
	case GL_DEBUG_SOURCE_THIRD_PARTY: {
		sourceString = "Third Party";
		break;
	}
	case GL_DEBUG_CATEGORY_OTHER_AMD:
	case GL_DEBUG_SOURCE_OTHER: {
		sourceString = "Other";
		break;
	}
	default: {
		sourceString = "Unknown";
		break;
	}
	}

	switch (type) {
	case GL_DEBUG_TYPE_ERROR: {
		typeString = "Error";
		break;
	}
	case GL_DEBUG_CATEGORY_DEPRECATION_AMD:
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: {
		typeString = "Deprecated Behavior";
		break;
	}
	case GL_DEBUG_CATEGORY_UNDEFINED_BEHAVIOR_AMD:
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: {
		typeString = "Undefined Behavior";
		break;
	}
	case GL_DEBUG_TYPE_PORTABILITY_ARB: {
		typeString = "Portability";
		break;
	}
	case GL_DEBUG_CATEGORY_PERFORMANCE_AMD:
	case GL_DEBUG_TYPE_PERFORMANCE: {
		typeString = "Performance";
		break;
	}
	case GL_DEBUG_CATEGORY_OTHER_AMD:
	case GL_DEBUG_TYPE_OTHER: {
		typeString = "Other";
		break;
	}
	default: {
		typeString = "Unknown";
		break;
	}
	}

	switch (severity) {
	case GL_DEBUG_SEVERITY_HIGH: {
		severityString = "High";
		break;
	}
	case GL_DEBUG_SEVERITY_MEDIUM: {
		severityString = "Medium";
		break;
	}
	case GL_DEBUG_SEVERITY_LOW: {
		severityString = "Low";
		break;
	}
	default: {
		severityString = "Unknown";
		break;
	}
	}

	stringStream << "OpenGL Error: " << msg;
	stringStream << " [Source = " << sourceString;
	stringStream << ", Type = " << typeString;
	stringStream << ", Severity = " << severityString;
	stringStream << ", ID = " << id << "]";

	return stringStream.str();
}

static void APIENTRY DebugCallbackAMD(GLuint id, GLenum category, GLenum severity, GLsizei length, const GLchar* message, GLvoid* userParam) {
	std::string error = FormatDebugOutput(category, category, id, severity, message);
	std::cout << error << std::endl;
}

static void APIENTRY DebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const GLvoid* userParam) {
	std::string error = FormatDebugOutput(source, type, id, severity, message);
	std::cout << error << std::endl;
}

int main(void){

	asteroids = vector<GameObject>();
	bullets = vector<Bullet>();

    // Initialise GLFW
    if( !glfwInit() )
    {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        getchar();
        return -1;
    }

#ifdef _DEBUG //enable opengl debug mode
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif
    
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    // Open a window
    window = glfwCreateWindow( 1024, 768, "3D Asteroids", NULL, NULL);
    if( window == NULL ){
        fprintf( stderr, "Failed to open GLFW window.\n" );
        getchar();
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
	glfwSetMouseButtonCallback(window, onMouseButton);
    
    // Initialize GLEW
    glewExperimental = true;
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
        return -1;
    }

#ifdef _DEBUG //register the callack functions
	// Query the OpenGL function to register your callback function.
	PFNGLDEBUGMESSAGECALLBACKPROC _glDebugMessageCallback = (PFNGLDEBUGMESSAGECALLBACKPROC)glfwGetProcAddress("glDebugMessageCallback");
	PFNGLDEBUGMESSAGECALLBACKARBPROC _glDebugMessageCallbackARB = (PFNGLDEBUGMESSAGECALLBACKARBPROC)glfwGetProcAddress("glDebugMessageCallbackARB");
	PFNGLDEBUGMESSAGECALLBACKAMDPROC _glDebugMessageCallbackAMD = (PFNGLDEBUGMESSAGECALLBACKAMDPROC)glfwGetProcAddress("glDebugMessageCallbackAMD");

	// Register your callback function.
	if (_glDebugMessageCallback != NULL) {
		_glDebugMessageCallback(DebugCallback, NULL);
	}
	else if (_glDebugMessageCallbackARB != NULL) {
		_glDebugMessageCallbackARB(DebugCallback, NULL);
	}
	else if (_glDebugMessageCallbackAMD != NULL) {
		_glDebugMessageCallbackAMD(DebugCallbackAMD, NULL);
	}

	// Enable synchronous callback. This ensures that your callback function is called
	// right after an error has occurred. This capability is not defined in the AMD
	// version.
	if ((_glDebugMessageCallback != NULL) || (_glDebugMessageCallbackARB != NULL)) {
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	}
#endif

	//FRAMEBUFFER
	//load shaders for post-processing
	GLuint postProgramID = LoadShaders("shaders/fbo.vert", "shaders/fbo.frag");
	GLuint fbTextureID = glGetUniformLocation(postProgramID, "fbTexture");
	GLuint brightTextureID = glGetUniformLocation(postProgramID, "brightTexture");

	//create the framebuffer
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	//create a texture for framebuffer
	glGenTextures(1, &texColorBuffer);
	glBindTexture(GL_TEXTURE_2D, texColorBuffer);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 10);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 1024, 768, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);

	//create texture forr bright spots as well
	GLuint brightColorBuffer;
	glGenTextures(1, &brightColorBuffer);
	glBindTexture(GL_TEXTURE_2D, brightColorBuffer);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 1024, 768, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, brightColorBuffer, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	GLenum DrawBuffers[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, DrawBuffers);

	//create depth buffer
	glGenRenderbuffers(1, &depthrenderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 1024, 768);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	//check that framebuffer is complete
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "framebuffer error " << status << "\n";
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//create square to render the framebuffer onto
	GLfloat fbQuadVerts[] = {
		-1.0f, -1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
		 1.0f,  1.0f, 0.0f
	};

	GLuint fbQuadVao;
	glGenVertexArrays(1, &fbQuadVao);
	glBindVertexArray(fbQuadVao);

	GLuint fbQuadVbo;
	glGenBuffers(1, &fbQuadVbo);
	glBindBuffer(GL_ARRAY_BUFFER, fbQuadVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(fbQuadVerts), fbQuadVerts, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//PING-PONG FBOs FOR GAUSSIAN BLUR
	GLuint pingPongFbos[2];
	GLuint pingPongTexs[2];
	glGenFramebuffers(2, pingPongFbos);
	glGenTextures(2, pingPongTexs);
	for (unsigned int i = 0; i < 2; i++) {
		glBindFramebuffer(GL_FRAMEBUFFER, pingPongFbos[i]);
		glBindTexture(GL_TEXTURE_2D, pingPongTexs[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 1024, 768, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingPongTexs[i], 0);
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	blurProgramID = LoadShaders("shaders/fbo.vert", "shaders/gauss.frag");
	GLuint gaussTextureID = glGetUniformLocation(blurProgramID, "gaussTexture");
	GLuint horizontalID = glGetUniformLocation(blurProgramID, "horizontal");

	//FBO SET UP END

	//TRANSFORM FEEDBACK SETUP

	GLchar *varyings[3] = {"outPos", "outVel", "outTime"};
	GLuint partProgID = LoadShadersWG("shaders/updateParticle.vert", "shaders/updateParticle.geom", NULL, varyings, 3);

	//create transform feedback buffers and vertex buffer objects
	Particle particles[MAX_PARTICLES] = {};
	GLuint particleTFBs[2];
	GLuint particleVAOs[2];
	GLuint particleVBOs[2];

	glGenTransformFeedbacks(2, particleTFBs);
	glGenVertexArrays(2, particleVAOs);
	glGenBuffers(2, particleVBOs);
	for (int i = 0; i < 2; i++) {
		glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, particleTFBs[i]);
		glBindVertexArray(particleVAOs[i]);
		glBindBuffer(GL_ARRAY_BUFFER, particleVBOs[i]);

		glBufferData(GL_ARRAY_BUFFER, sizeof(particles), particles, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		char *offset = 0;
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), offset); // position
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), offset + 12); // velocity
		glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), offset + 24); // time

		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, particleVBOs[i]);
	}
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);

	//

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    // Hide the mouse and enable unlimited mouvement
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    // Set the mouse at the center of the screen
    glfwPollEvents();
    glfwSetCursorPos(window, 1024/2, 768/2);
    
    // blue background
    glClearColor(0.0f, 0.0f, 0.1f, 0.0f);
    
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);
    
    // Cull triangles which normal is not towards the camera
    glEnable(GL_CULL_FACE);
    
    // Create and compile our GLSL program from the shaders
    programID = LoadShaders( "shaders/basic.vert", "shaders/basic.frag" );//add the path u saved your basis.vert/basic.frag in
    
    // Get handles for uniforms
	modelID = glGetUniformLocation(programID, "model");
	viewID = glGetUniformLocation(programID, "view");
	projectionID = glGetUniformLocation(programID, "projection");
	lightColorID = glGetUniformLocation(programID, "lightColor");
	lightPosID = glGetUniformLocation(programID, "lightPos");
	viewPosID = glGetUniformLocation(programID, "viewPos");
	exponentID = glGetUniformLocation(programID, "exponent");
	specularMultID = glGetUniformLocation(programID, "specularMult");
    
    // Load the texture
    GLuint Texture = loadDDS("assets/uvmap.DDS");//add the path u saved your file in
    
    // Get a handle for our "myTextureSampler" uniform
    textureID  = glGetUniformLocation(programID, "myTextureSampler");
    
    // Read .obj file
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;
    bool res = loadOBJ("assets/rock.obj", vertices, uvs, normals);//add the path u saved your file in

	rock = new Mesh(vertices, uvs, normals, Texture);

	//create a bunch of random asteroids for demonstration
	for (int i = 0; i < 20; i++) {
		GameObject current = GameObject(rock);
		current.scale = (rand() % 9) / 3.0;
		current.position = vec3((rand() % 200) / 2.0 - 50.0, (rand() % 200) / 2.0 - 50.0, (rand() % 100) / 2.0 - 25.0);
		current.addRotation(glm::normalize(vec3(rand() % 10 - 5, rand() % 10 - 5, rand() % 10 - 5)), rand() % 90 - 45);
		current.hp = 100 * current.scale;
		asteroids.push_back(current);
	}

	double lastTime = glfwGetTime();

	bool firstTFB = true;
	bool firstTransform = true;

    do{
        //bind the framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		glViewport(0, 0, 1024, 768);
		glEnable(GL_DEPTH_TEST);

        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Use our shader
        glUseProgram(programID);
        
        // Compute the MVP matrix from keyboard and mouse input
        computeMatricesFromInputs();
        glm::mat4 ProjectionMatrix = getProjectionMatrix();
        glm::mat4 ViewMatrix = getViewMatrix();
        
		glUniformMatrix4fv(viewID, 1, GL_FALSE, &ViewMatrix[0][0]);
		glUniformMatrix4fv(projectionID, 1, GL_FALSE, &ProjectionMatrix[0][0]);

		glm::vec3 lightColor = glm::vec3(1000.0, 1000.0, 1000.0);
		glUniform3fv(lightColorID, 1, &lightColor[0]);

		glm::vec3 lightPos = glm::vec3(0.0, 0.0, 0.0);
		glUniform3fv(lightPosID, 1, &lightPos[0]);

		glm::vec3 viewPos = getViewPos();
		glUniform3fv(viewPosID, 1, &viewPos[0]);

		glUniform1f(exponentID, 100.0);
		glUniform1f(specularMultID, 0.5);
        
		//render the game objects
		double time = glfwGetTime();

		//render asteroids
		for (int i = 0; i < asteroids.size(); i++) {
			GameObject *current=&asteroids[i];
			current->update(time - lastTime);
			if (current->isAlive()) {
				renderGameObject(current);
			}else {
				asteroids.erase(asteroids.begin() + i);
			}
		}

		//render bullets
		for (int i = 0; i < bullets.size(); i++) {
			Bullet *current = &bullets[i];
			current->update(time - lastTime);
			if (current->isAlive()) {
				renderGameObject(current);
			}else {
				bullets.erase(bullets.begin() + i);
			}
		}

		//RENDER PARTICLES

		//
		glUseProgram(partProgID);
		glEnable(GL_RASTERIZER_DISCARD);

		glUniform1f(glGetUniformLocation(partProgID, "dTime"), time - lastTime);
		glBindBuffer(GL_ARRAY_BUFFER, particleVBOs[firstTFB]);
		glBindVertexArray(particleVAOs[firstTFB]);

		//glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, particleTFBs[!firstTFB]);
		glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, particleTFBs[!firstTFB]);

		glBeginTransformFeedback(GL_POINTS);
		if (firstTransform) {
			glDrawArrays(GL_POINTS, 0, 0);
			firstTransform = false;
		}else {
			glDrawTransformFeedback(GL_POINTS, particleTFBs[firstTFB]);
		}
		glEndTransformFeedback();

		firstTFB = !firstTFB;
		glDisable(GL_RASTERIZER_DISCARD);

		//blur the intermediate results
		glUseProgram(blurProgramID);
		bool horizontal = true;
		bool first_iteration = true;
		glBindVertexArray(fbQuadVao);

		for (unsigned int i = 0; i < 10; i++){
			glBindFramebuffer(GL_FRAMEBUFFER, pingPongFbos[horizontal]);
			glUniform1i(horizontalID, horizontal);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, first_iteration ? brightColorBuffer : pingPongTexs[!horizontal]);
			glUniform1i(fbTextureID, 0);

			glDrawArrays(GL_TRIANGLES, 0, 6);
			horizontal = !horizontal;
			if (first_iteration) {
				first_iteration = false;
			}
		}
		glBindVertexArray(0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		//render the framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, 1024, 768);
		glClearColor(0.00f, 0.00f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);

		glUseProgram(postProgramID);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texColorBuffer);
		glUniform1i(fbTextureID, 0);
		glGenerateMipmap(GL_TEXTURE_2D); //create mipmap so that its maximum level can be accessed in shader as average

		//bind the blurred bright texture
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, pingPongTexs[1]);
		glUniform1i(brightTextureID, 1);

		glBindVertexArray(fbQuadVao);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		glBindVertexArray(0);

		//check for collision
		for (int i = 0; i < asteroids.size(); i++) {
			GameObject *asteroid = &asteroids[i];

			for (int j = 0; j < bullets.size(); j++) {
				Bullet *bullet = &bullets[j];
				if (asteroid->collides(bullet)) {
					asteroid->hp -= bullet->damage;
					bullet->age += bullet->timeToLive; //dirty hack!
				}
			}
		}

		lastTime = glfwGetTime();
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    } // Check if the ESC key was pressed or the window was closed
    while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
          glfwWindowShouldClose(window) == 0 );
    
    // Cleanup VBO and shader
    /*glDeleteBuffers(1, &vertexbuffer);
    glDeleteBuffers(1, &uvbuffer);
	glDeleteBuffers(1, &normalBuffer);*/
    glDeleteProgram(programID);
    glDeleteTextures(1, &textureID);
	delete rock;
	glDeleteFramebuffers(1, &framebuffer);
    //glDeleteVertexArrays(1, &VertexArrayID);
    
    // Close OpenGL window and terminate GLFW
    glfwTerminate();
    
    return 0;
}

