#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormal_modelspace;
layout(location = 3) in vec3 vertexTangent_modelspace;
layout(location = 4) in vec3 vertexBitangent_modelspace;

// Output data ; will be interpolated for each fragment.
out vec2 UV;
out vec3 worldPos;
out vec3 EyeDirection_cameraspace;
out vec3 LightDirection_cameraspace;

out vec3 LightDirection_tangentspace;
out vec3 EyeDirection_tangentspace;

// Values that stay constant for the whole mesh.
//uniform mat4 MVP;
uniform mat4 view;
uniform mat4 model;
uniform mat3 MV3x3;
uniform vec3 lightPos;
uniform mat4 projection;

void main(){
// Output position of the vertex, in clip space : MVP * position
	gl_Position =  projection * view * model * vec4(vertexPosition_modelspace,1);

	// Position of the vertex, in worldspace : model * position
	worldPos = (model * vec4(vertexPosition_modelspace,1)).xyz;

	// Vector that goes from the vertex to the camera, in camera space.
	// In camera space, the camera is at the origin (0,0,0).
	vec3 vertexPosition_cameraspace = ( view * model * vec4(vertexPosition_modelspace,1)).xyz;
	EyeDirection_cameraspace = vec3(0,0,0) - vertexPosition_cameraspace;

	// Vector that goes from the vertex to the light, in camera space. model is ommited because it's identity.
	vec3 LightPosition_cameraspace = ( view * vec4(lightPos,1)).xyz;
	LightDirection_cameraspace = LightPosition_cameraspace + EyeDirection_cameraspace;
	
	// UV of the vertex. No special space for this one.
	UV = vertexUV;
	
	// model to camera = ModelView
	vec3 vertexTangent_cameraspace   = MV3x3 * vertexTangent_modelspace;
	vec3 vertexBitangent_cameraspace = MV3x3 * vertexBitangent_modelspace;
	vec3 vertexNormal_cameraspace    = MV3x3 * vertexNormal_modelspace;
	
	mat3 TBN = transpose(mat3(vertexTangent_cameraspace, vertexBitangent_cameraspace, vertexNormal_cameraspace));

	LightDirection_tangentspace = TBN * LightDirection_cameraspace;
	EyeDirection_tangentspace   = TBN * EyeDirection_cameraspace;
}

