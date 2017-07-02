#version 330

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 normal;

out vec2 UV;
out vec3 N;
out vec3 worldPos;
out vec3 EyeDirection_cameraspace;
out vec3 Normal_cameraspace;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(){
	gl_Position =  projection * view * model * vec4(vertexPosition_modelspace,1);
	UV = vertexUV;
	N = normalize((transpose(inverse(model)) * vec4(normal, 1.0)).xyz); //calculate inverse transpose model matrix in program itself and then assign to uniform instead
	worldPos = (model * vec4(vertexPosition_modelspace, 1.0)).xyz;


	// Vector that goes from the vertex to the camera, in camera space.
	// In camera space, the camera is at the origin (0,0,0).
	vec3 vertexPosition_cameraspace = ( view * model * vec4(vertexPosition_modelspace,1)).xyz;
	EyeDirection_cameraspace = (vec3(0,0,0) - vertexPosition_cameraspace);

	// Normal of the the vertex, in camera space
	Normal_cameraspace = ( view * model * vec4(normal,0)).xyz; // Only correct if ModelMatrix does not scale the model ! Use its inverse transpose if not.
}

