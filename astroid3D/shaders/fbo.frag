#version 330

//#extension GL_ARB_texture_query_lod : enable

in vec2 UV;

uniform sampler2D fbTexture;
uniform sampler2D brightTexture;

out vec3 color;

void main(){

	vec3 avg=textureLod(fbTexture, UV, 10.0).rgb;
	float lum=0.2126*avg.r + 0.7152*avg.g + 0.0722*avg.b;
	lum*=3.0;
	if(lum<0.4){
		lum=0.4;
	}else if(lum>1.0){
		lum=0.4;
	}else if(isnan(lum)){
		lum=0.5;
	}
	float T=pow(lum, -1.0);
	vec3 temp=texture(fbTexture, UV).rgb+texture(brightTexture, UV).rgb;
	
	color.r=1.0-exp(-T*temp.r);
	color.g=1.0-exp(-T*temp.g);
	color.b=1.0-exp(-T*temp.b);
}