#version 330

in vec2 UV;

uniform sampler2D gaussTexture;
uniform bool horizontal;

out vec3 color;
//const float weights[7]=float[](0.106595, 0.140367, 0.165569, 0.174938, 0.165569, 0.140367, 0.106595);
const float weights[11]=float[](0.035822, 0.05879, 0.086425, 0.113806, 0.13424, 0.141836, 0.13424, 0.113806, 0.086425, 0.05879, 0.035822);

void main(){
	vec2 pxlSize=vec2(1.0)/textureSize(gaussTexture, 0);
	int center=weights.length()/2;
	
	if(horizontal){
		for(int i=0; i<weights.length(); i++){
			color+=weights[i] * texture(gaussTexture, UV+vec2((i-center)*pxlSize.x, 0.0)).rgb;
		}
	}else{
		for(int i=0; i<weights.length(); i++){
			color+=weights[i] * texture(gaussTexture, UV+vec2(0.0, (i-center)*pxlSize.y)).rgb;
		}
	}
}