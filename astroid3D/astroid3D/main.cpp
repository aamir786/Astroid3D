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
#include "text2D.hpp"

const int MAX_PARTICLES = 10000;

GLFWwindow* window;

Mesh *rock;

GLuint programID;
GLuint programIDFCulling;
GLuint programIDContours;
GLuint programIDNormalMapping;
GLuint blurProgramID;

GLuint framebuffer;
GLuint texColorBuffer;
GLuint depthrenderbuffer;

//ids of uniforms
GLuint modelID, viewID, projectionID, lightColorID, lightPosID, viewPosID, exponentID, textureID;
GLuint ProjectionMatrixID, ModelMatrixID, MatrixID, LightID, ViewMatrixID, ModelView3x3MatrixID;

GLuint DiffuseTextureID;
GLuint NormalTextureID;   
GLuint SpecularTextureID; 


vector<Bullet> bullets; //list of bullets
vector<GameObject> asteroids; //list of asteroids
bool m_bIsWireFrame = false;
bool m_bIsCulling = false;
short nFunction = 0;
bool bFinished = false;
void renderMesh(Mesh *mesh) {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mesh->Dtexture);
	
	if(nFunction == 3){
		// Set our "DiffuseTextureSampler" sampler to user Texture Unit 0
		glUniform1i(DiffuseTextureID, 0);

		// Bind our normal texture in Texture Unit 1
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, mesh->Ntexture);
		glUniform1i(NormalTextureID, 1);

		// Bind our Specular texture in Texture Unit 2
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, mesh->Stexture);
		glUniform1i(SpecularTextureID, 2);
	}
	else{
		glUniform1i(textureID, 0);
	}
	glBindVertexArray(mesh->vao);

	if(m_bIsWireFrame){
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	glDrawArrays(GL_TRIANGLES, 0, mesh->vertices.size());
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBindVertexArray(0);
}

void renderGameObject(GameObject *obj)
{
	if(nFunction == 3) {
		glm::mat4 ProjectionMatrix = getProjectionMatrix();
		glm::mat4 ViewMatrix = getViewMatrix();
		glm::mat4 ModelViewMatrix = ViewMatrix * obj->getModelMatrix()[0][0];
		glm::mat3 ModelView3x3Matrix = glm::mat3(ModelViewMatrix);
		//glm::mat4 MVP = ProjectionMatrix * ViewMatrix * obj->getModelMatrix()[0][0];
		//glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &obj->getModelMatrix()[0][0]);//
		glUniformMatrix3fv(ModelView3x3MatrixID, 1, GL_FALSE, &ModelView3x3Matrix[0][0]);
	}
	else {
		glUniformMatrix4fv(modelID, 1, GL_FALSE, &obj->getModelMatrix()[0][0]);
	}
	renderMesh(obj->mesh);
}

/*
	GLFW mouse button event listener
*/
void onMouseButton(GLFWwindow* window, int button, int action, int mods) {
	if(bFinished) return;

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		Bullet foo = Bullet(3, 10, rock);
		foo.position = getViewPos();
		foo.velocity = 10.0f * getViewDirection();
		foo.scale = 0.05; //temporarily, until we get bullet models
		bullets.push_back(foo);
	}
}

void onKeyBoard_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if(bFinished) return;
	if (key == GLFW_KEY_F3 && action == GLFW_PRESS){
		m_bIsWireFrame = !m_bIsWireFrame;
	}
	if (key == GLFW_KEY_F6 && action == GLFW_PRESS){ //Edge Detection
		nFunction = 2;
	}
	if (key == GLFW_KEY_F7 && action == GLFW_PRESS){ //Normal Mapping
		nFunction = 3;
	}
	if (key == GLFW_KEY_F8 && action == GLFW_PRESS){
		nFunction = 1;
		m_bIsCulling = !m_bIsCulling;
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

    // Initialize GLFW
    if( !glfwInit() ) {
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
    if(window == NULL) {
        fprintf( stderr, "Failed to open GLFW window.\n" );
        getchar();
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
	glfwSetMouseButtonCallback(window, onMouseButton);
	glfwSetKeyCallback(window, onKeyBoard_callback);
    
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
	//load shader for post-processing
	GLuint postProgramID   = LoadShaders("shaders/fbo.vert", "shaders/fbo.frag");
	GLuint fbTextureID     = glGetUniformLocation(postProgramID, "fbTexture");
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

	//create texture for bright spots as well
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
    // Hide the mouse and enable unlimited movement
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
	
	programID = LoadShaders( "shaders/basic.vert", "shaders/basic.frag" );//add the path u saved your basis.vert/basic.frag in
    
    // Create and compile our GLSL program from the shaders
	GLchar *varyings1[3] = {"UVgout", "Ngout", "worldPosgout"};
    programIDFCulling = LoadShadersWG( "shaders/frustumCulling.vert", "shaders/frustumCulling.geom", "shaders/frustumCulling.frag", varyings1, 3);//Frustum Culling

	GLchar *varyings2[5] = {"UVgout", "Ngout", "worldPosgout", "EyeDirection_cameraspacegout", "Normal_cameraspacegout"};
	programIDContours = LoadShadersWG( "shaders/Contours.vert", "shaders/Contours.geom", "shaders/Contours.frag", varyings2, 5);//Edge Detection

//	GLuint VertexArrayIDNormalMapping;
//	glGenVertexArrays(1, &VertexArrayIDNormalMapping);
//	glBindVertexArray(VertexArrayIDNormalMapping);

	programIDNormalMapping = LoadShaders( "shaders/NormalMapping.vert", "shaders/NormalMapping.frag");//Normal Mapping
	
	// Load the texture
	GLuint RockTexture     = loadDDS("assets/uvmap.DDS");
	GLuint NormalTexture   = loadDDS("assets/NormalMap1.dds");//NormalMap.DDS");
	GLuint SpecularTexture = loadDDS("assets/specular1.DDS");//specular.dds");

	LightID              = glGetUniformLocation(programIDNormalMapping, "lightPos");
//	MatrixID             = glGetUniformLocation(programIDNormalMapping, "MVP");
	ViewMatrixID         = glGetUniformLocation(programIDNormalMapping, "view");
	ModelMatrixID        = glGetUniformLocation(programIDNormalMapping, "model");
	ProjectionMatrixID   = glGetUniformLocation(programIDNormalMapping, "projection");
	ModelView3x3MatrixID = glGetUniformLocation(programIDNormalMapping, "MV3x3");
	DiffuseTextureID     = glGetUniformLocation(programIDNormalMapping, "DiffuseTextureSampler");
	NormalTextureID      = glGetUniformLocation(programIDNormalMapping, "NormalTextureSampler");
	SpecularTextureID    = glGetUniformLocation(programIDNormalMapping, "SpecularTextureSampler");

	
    // Get handles for uniforms
	modelID        = glGetUniformLocation(programID, "model");
	viewID         = glGetUniformLocation(programID, "view");
	projectionID   = glGetUniformLocation(programID, "projection");
	lightColorID   = glGetUniformLocation(programID, "lightColor");
	lightPosID     = glGetUniformLocation(programID, "lightPos");
	viewPosID      = glGetUniformLocation(programID, "viewPos");
	exponentID     = glGetUniformLocation(programID, "exponent");
	textureID      = glGetUniformLocation(programID, "myTextureSampler");
            
    // Read .obj file
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;
    bool res = loadOBJ("assets/rock.obj", vertices, uvs, normals);//add the path u saved your file in
	if(!res) {
		fprintf(stderr, "Failed to load rock.obj");
		getchar();
		glfwTerminate();
		return -1;
	}
	rock = new Mesh(vertices, uvs, normals, RockTexture, NormalTexture, SpecularTexture);

	//create a bunch of random asteroids for demonstration
	for (int i = 0; i < 20; i++) {
		GameObject current = GameObject(rock);
		current.scale = (rand() % 9) / 3.0;
		float axis = (rand() % 200) / 2.0 - 50.0;
		float z    = (rand() % 100) / 2.0 - 25.0;
		current.position = vec3(axis, axis, z);
		axis        = rand() % 10 - 5;
		float angle = rand() % 90 - 45;
		current.addRotation(glm::normalize(vec3(axis, axis, axis)), angle);
		current.hp = current.defaultHp = 100 * current.scale;
		asteroids.push_back(current);
	}
	
	// The VBO containing the 4 vertices of the particles.

	GLuint VertexArrayIDBillboard;
	glGenVertexArrays(1, &VertexArrayIDBillboard);
	glBindVertexArray(VertexArrayIDBillboard);

	static const GLfloat g_vertex_buffer_data[] = { 
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		-0.5f,  0.5f, 0.0f,
		0.5f,  0.5f, 0.0f,
	};
	GLuint billboard_vertex_buffer;
	glGenBuffers(1, &billboard_vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_DYNAMIC_DRAW);

	// Create and compile our GLSL program from the shaders
	GLuint programIDBillboard = LoadShaders( "shaders/Billboard.vert", "shaders/Billboard.frag" );

	// Vertex shader
	GLuint CameraRight_worldspace_ID = glGetUniformLocation(programIDBillboard, "CameraRight_worldspace");
	GLuint CameraUp_worldspace_ID    = glGetUniformLocation(programIDBillboard, "CameraUp_worldspace");
	GLuint ViewProjMatrixID          = glGetUniformLocation(programIDBillboard, "VP");
	GLuint BillboardPosID            = glGetUniformLocation(programIDBillboard, "BillboardPos");
	GLuint BillboardSizeID           = glGetUniformLocation(programIDBillboard, "BillboardSize");
	GLuint LifeLevelID               = glGetUniformLocation(programIDBillboard, "LifeLevel");
	GLuint TextureIDBillboard        = glGetUniformLocation(programIDBillboard, "myTextureSamplerBillboard");
	GLuint TextureBillboard          = loadDDS("assets/Billboard.DDS");

	double lastTime = glfwGetTime();

	bool firstTFB = true;
	bool firstTransform = true;

	int  nbFrames = 0;
	double lastTime1 = glfwGetTime();

	short countFPS = 0, maxAvgFPS = 60, nFPS_uf = 0, nFPS = 0; //nFPS_unfiltered, nFPS is Filtered

	char strTitle[32];
	initText2D("assets/Holstein.DDS");
    do {
		sprintf_s(strTitle, "FPS : %d, Culling : %d", nFPS, m_bIsCulling ? 1: 0);
		glfwSetWindowTitle(window, strTitle);

		//printf("FPS : %d\n", nFPS);
		// Measure speed
		nbFrames++;
		if ( glfwGetTime() - lastTime1 >= 1.0 ){ // If last prinf() was more than 1 sec ago
			// printf and reset timer
			printf("%f ms/frame, Culling : %d\n", 1000.0/double(nbFrames), m_bIsCulling ? 1: 0);
			//printf("FPS : %d\n", nFPS);
			//printf("FPS_uf : %d\n", nFPS_uf);
			nbFrames = 0;
			lastTime1 += 1.0;
		}

        //bind the framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		glViewport(0, 0, 1024, 768);
		glEnable(GL_DEPTH_TEST);

        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Compute the MVP matrix from keyboard and mouse input
		if(!bFinished) computeMatricesFromInputs();
		glm::mat4 ProjectionMatrix = getProjectionMatrix();
		glm::mat4 ViewMatrix = getViewMatrix();
		glm::vec3 lightPos = glm::vec3(0.0, 0.0, 0.0);

#pragma region ModelDrawingShader
        // Use our shader
		switch(nFunction) {
			case 0: //Without culling without any effect
				glUseProgram(programID);
				break;
			case 1: //Culling
				glUseProgram(programIDFCulling);
				break;
			case 2: //Contours (edge detection)
				glUseProgram(programIDContours);
				break;
			case 3: //Normal mapping/ tangent space calculation
				glUseProgram(programIDNormalMapping);
				break;
			default:
				glUseProgram(programID);
		}
		if(nFunction == 3) {
			glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);//
			glUniformMatrix4fv(ProjectionMatrixID, 1, GL_FALSE, &ProjectionMatrix[0][0]);//
			glUniform3fv(LightID, 1, &lightPos[0]);//
		}
		else {
			glUniformMatrix4fv(viewID, 1, GL_FALSE, &ViewMatrix[0][0]);//
			glUniformMatrix4fv(projectionID, 1, GL_FALSE, &ProjectionMatrix[0][0]);//
			glm::vec3 lightColor = glm::vec3(1000.0, 1000.0, 1000.0);
			glUniform3fv(lightColorID, 1, &lightColor[0]);
			glUniform3fv(lightPosID, 1, &lightPos[0]);//
			glm::vec3 viewPos = getViewPos();
			glUniform3fv(viewPosID, 1, &viewPos[0]);

			glUniform1f(exponentID, 100.0);
		}        
		//render the game objects
		double time = glfwGetTime();

		//render asteroids
		for (int i = 0; i < asteroids.size(); i++) {
			GameObject *current=&asteroids[i];
			if (current->isAlive()) {
				current->update(time - lastTime);
				renderGameObject(current);
			}
			else {
				asteroids.erase(asteroids.begin() + i);
			}
		}

		//render bullets
		for (int i = 0; i < bullets.size(); i++) {
			Bullet *current = &bullets[i];
			if (current->isAlive()) {
				current->update(time - lastTime);
				renderGameObject(current);
			}
			else {
				bullets.erase(bullets.begin() + i);
			}
		}
#pragma endregion ModelDrawingShader

#pragma region ParticlesShader
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
		}
		else {
			glDrawTransformFeedback(GL_POINTS, particleTFBs[firstTFB]);
		}
		glEndTransformFeedback();

		firstTFB = !firstTFB;
		glDisable(GL_RASTERIZER_DISCARD);
#pragma endregion ParticlesShader		

#pragma region BlurShader
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
		
		//render the framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, 1024, 768);
		glClearColor(0.00f, 0.00f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);

#pragma endregion BlurShader

#pragma region postProgramShader
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
		static float x1 = -1000.0, y1 = -1000.0, z1 = 0.0;
		static float fLifeLevel = 1.0f;

		for (int i = 0; i < asteroids.size(); i++) {
			GameObject *asteroid = &asteroids[i];
			for (int j = 0; j < bullets.size(); j++) {
				Bullet *bullet = &bullets[j];
				if (asteroid->collides(bullet)) {
					asteroid->hp -= bullet->damage;
					bullet->age += bullet->timeToLive; //dirty hack!
					if(asteroid->hp <= 0.0f) {
						x1 = y1 = -1000.0f;
					}
					else {
						x1 = asteroid->position.x;
						y1 = asteroid->position.y;
						z1 = asteroid->position.z;
						fLifeLevel = asteroid->hp/asteroid->defaultHp;
					}
					break;
				}
			}
		}
#pragma endregion postProgramShader

		char text[256];
		float fTimeOut = 180.0f - glfwGetTime();
		if(fTimeOut <= 0.0f) {
			bFinished = true;
			fTimeOut = 0.0f;
			printText2D("Game End", 1024/2-200, 768/2-50, 30);
		}
		sprintf(text,"Time : %.2f sec", fTimeOut);
		printText2D(text, 10, 550, 20);
		sprintf(text,"Asteroids : %d", asteroids.size() );
		printText2D(text, 10, 510, 20);		

#pragma region BillBoardShader
		glm::vec3 CameraPosition(glm::inverse(ViewMatrix)[3]);
		glm::mat4 ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// Use our shader
		glUseProgram(programIDBillboard);

		// Bind our texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, TextureBillboard);
		glUniform1i(TextureIDBillboard, 0);

		glUniform3f(CameraRight_worldspace_ID, ViewMatrix[0][0], ViewMatrix[1][0], ViewMatrix[2][0]);
		glUniform3f(CameraUp_worldspace_ID   , ViewMatrix[0][1], ViewMatrix[1][1], ViewMatrix[2][1]);

		glUniform3f(BillboardPosID, x1, y1, z1);
		glUniform2f(BillboardSizeID, 1.0f, 0.125f);     // and 1m*12cm, because it matches its 256*32 resolution =)

		glUniform1f(LifeLevelID, fLifeLevel);

		glUniformMatrix4fv(ViewProjMatrixID, 1, GL_FALSE, &ViewProjectionMatrix[0][0]);
		glBindVertexArray(VertexArrayIDBillboard);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glDisableVertexAttribArray(0);
		glBindVertexArray(0);
#pragma endregion BillBoardShader
		
		nFPS_uf = (short)(1.0/(glfwGetTime() - lastTime));
		
		lastTime = glfwGetTime();

		// FPS calculation using moving average
		if(countFPS < maxAvgFPS) countFPS++;
		nFPS = (short)(((countFPS-1)*nFPS+nFPS_uf)/(float)countFPS); 
		//Alternatively, Smoothing Average, Last Value:90% , New Value : 10%
		//float smoothing = 0.9;
		//measurement = (measurement * smoothing) + (current * (1.0-smoothing))
		
        glfwSwapBuffers(window);
        glfwPollEvents();
    } // Check if the ESC key was pressed or the window was closed
    while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
          glfwWindowShouldClose(window) == 0 );
    
    // Cleanup VBO and shader

    glDeleteProgram(programID);
	glDeleteProgram(programIDFCulling);
	glDeleteProgram(programIDContours);
	glDeleteProgram(programIDNormalMapping);
    glDeleteTextures(1, &textureID);
	glDeleteTextures(1, &TextureIDBillboard);
	glDeleteVertexArrays(1, &VertexArrayIDBillboard);
	delete rock;
	glDeleteFramebuffers(1, &framebuffer);
    //glDeleteVertexArrays(1, &VertexArrayIDNormalMapping);
	glDeleteTextures(1, &RockTexture);
	glDeleteTextures(1, &NormalTexture);
	glDeleteTextures(1, &SpecularTexture);
    
    // Close OpenGL window and terminate GLFW
    glfwTerminate();
    
    return 0;
}

