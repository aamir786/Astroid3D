#version 330 core

// Interpolated values from the vertex shaders
in vec2 UV;

// Ouput data
out vec4 color;

uniform sampler2D myTextureSamplerBillboard;

uniform float LifeLevel;

void main(){
	// Output color = color of the texture at the specified UV
	color = texture( myTextureSamplerBillboard, UV );
	
	// Hardcoded life level, should be in a separate texture.
	if (UV.x < LifeLevel && UV.y > 0.3 && UV.y < 0.7 && UV.x > 0.04 ){
		if(LifeLevel < 0.5f)
			color = vec4(0.8, 0.2, 0.2, 1.0); // Red
		else if(LifeLevel < 0.25f)
			color = vec4(1.0, 0.0, 0.0, 1.0); // Red
		else if(LifeLevel < 0.05f)
			color = vec4(1.0, 0.0, 0.0, 1.0); // Red
		else
			color = vec4(0.2, 0.8, 0.2, 1.0); // Opaque green
	}
}