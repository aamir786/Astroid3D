//
//  controls.cpp
//  3d_Astroid
//
//
#include <GLFW/glfw3.h>
#include <stdio.h>
extern GLFWwindow* window;

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include "controls.hpp"

glm::mat4 ViewMatrix;
glm::mat4 ProjectionMatrix;
// Initial position : on +Z
glm::vec3 position = glm::vec3(0, 0, 5);
glm::vec3 velocity = glm::vec3(0, 0, 0); //velocity vector
glm::vec3 direction;

float mju = 0.99; //friction coefficient

glm::mat4 getViewMatrix(){
    return ViewMatrix;
}
glm::mat4 getProjectionMatrix(){
    return ProjectionMatrix;
}
glm::vec3 getViewPos() {
	return position;
}
glm::vec3 getViewDirection() {
	return direction;
}


// Initial horizontal angle : toward -Z
float horizontalAngle = 3.14f;
// Initial vertical angle : none
float verticalAngle = 0.0f;
// Initial Field of View
float initialFoV = 45.0f;

float speed = 0.333f;
float mouseSpeed = 0.005f;



void computeMatricesFromInputs(){
    
    // glfwGetTime is called only once, the first time this function is called
    static double lastTime = glfwGetTime();
    
    // Compute time difference between current and last frame
    double currentTime = glfwGetTime();
    float deltaTime = float(currentTime - lastTime);
    
    // Get mouse position
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    
    // Reset mouse position for next frame
    glfwSetCursorPos(window, 1024/2, 768/2);
    
    // Compute new orientation
    horizontalAngle += mouseSpeed * float(1024/2 - xpos );
    verticalAngle   += mouseSpeed * float( 768/2 - ypos );
    
    // Direction : Spherical coordinates to Cartesian coordinates conversion
    direction= glm::vec3(
                        cos(verticalAngle) * sin(horizontalAngle),
                        sin(verticalAngle),
                        cos(verticalAngle) * cos(horizontalAngle)
                        );
    
    // Right vector
    glm::vec3 right = glm::vec3(
                                sin(horizontalAngle - 3.14f/2.0f),
                                0,
                                cos(horizontalAngle - 3.14f/2.0f)
                                );
    
    // Down vector
    glm::vec3 up = glm::cross( right, direction );
    
    // Move forward
    if (glfwGetKey( window, GLFW_KEY_W ) == GLFW_PRESS){
        velocity += direction * deltaTime * speed;
    }
    // Move backward
    if (glfwGetKey( window, GLFW_KEY_S ) == GLFW_PRESS){
        velocity -= direction * deltaTime * speed;
    }
    // Strafe right
    if (glfwGetKey( window, GLFW_KEY_D ) == GLFW_PRESS){
        velocity += right * deltaTime * speed;
    }
    // Strafe left
    if (glfwGetKey( window, GLFW_KEY_A ) == GLFW_PRESS){
        velocity -= right * deltaTime * speed;
    }
	// Move down
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
		velocity -= up * deltaTime * speed;
	}
	// Move down
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		velocity += up * deltaTime * speed;
	}

	position += velocity;
	velocity = velocity*mju;

    
    float FoV = initialFoV;// - 5 * glfwGetMouseWheel();
    
    ProjectionMatrix = glm::perspective(FoV, 4.0f / 3.0f, 0.1f, 100.0f);
    
    // Camera matrix
    ViewMatrix = glm::lookAt(
                             position,           // Camera is here
                             position+direction, // the same position, plus "direction"
                             up                  // Head is up (set to 0,-1,0 to look upside-down)
                            );
    
    // For the next frame, the "last time" will be "now"
    lastTime = currentTime;
}
