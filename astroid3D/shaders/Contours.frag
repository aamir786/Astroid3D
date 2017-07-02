#version 330

in vec2 UVgout;
in vec3 Ngout;        //normal
in vec3 worldPosgout; //position of fragment in world coordinates
in vec3 EyeDirection_cameraspacegout;
in vec3 Normal_cameraspacegout;

layout(location=0) out vec3 color;
layout(location=1) out vec3 brightColor;

uniform sampler2D myTextureSampler;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

uniform float exponent; //phong exponent

void main(){

	vec3 L=lightPos-worldPosgout; //light vector
	vec3 lc=lightColor/(dot(L, L)); //light intensity decreases with squared distance
	L=normalize(L);
	vec3 V=normalize(viewPos-worldPosgout); //view vector
	vec3 H=normalize(L+V); //blinn halfway vector
	
	color = lc*(dot(L, Ngout)*texture(myTextureSampler, UVgout).rgb + pow(dot(Ngout, H), exponent));

	// Normal of the computed fragment, in camera space
	vec3 c = normalize(EyeDirection_cameraspacegout);
	vec3 n = normalize(Normal_cameraspacegout);
	float CameraFacingPercentage = dot(c, n)+0.25;
	color = (color * CameraFacingPercentage) + vec3(0.5, 0.0, 0.0) * (1.0 - CameraFacingPercentage);

//Different Effect Only Hilight Outlines
//	color = (dot(c, n) > 0.2) ? color : vec3(0.5, 0.0, 0.0);

	if(dot(color, vec3(0.2126, 0.7152, 0.0722))>1.0) {
		brightColor=color;
	}
	else {
		brightColor=vec3(0.0);
	}
}
