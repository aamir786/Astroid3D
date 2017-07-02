#version 330 core

// Interpolated values from the vertex shaders
in vec2 UV;
in vec3 worldPos;
in vec3 EyeDirection_cameraspace;
in vec3 LightDirection_cameraspace;

in vec3 LightDirection_tangentspace;
in vec3 EyeDirection_tangentspace;

// Ouput data
out vec3 color;

// Values that stay constant for the whole mesh.
uniform sampler2D DiffuseTextureSampler;
uniform sampler2D NormalTextureSampler;
uniform sampler2D SpecularTextureSampler;
uniform mat4 view;
uniform mat4 model;
uniform mat3 MV3x3;
uniform vec3 lightPos;

void main(){
////////
//	vec3 L=lightPos-worldPos; //light vector
//	vec3 LightColor = vec3(1,1,1);
//	vec3 lc=LightColor/(dot(L, L)); //light intensity decreases with squared distance	
//	L=normalize(L);
//	vec3 V=normalize(viewPos-worldPos); //view vector
//	vec3 H=normalize(L+V); //blinn halfway vector
//	color = lc*(dot(L, Ngout)*texture(myTextureSampler, UVgout).rgb + pow(dot(Ngout, H), exponent));
//	if(dot(color, vec3(0.2126, 0.7152, 0.0722))>1.0){
//		brightColor=color;
//	}
//	else{
//		brightColor=vec3(0.0);
//	}
////////
	// Light emission properties
	// You probably want to put them as uniforms
	vec3 LightColor = vec3(1, 1, 1);	
	float LightPower = 100.0;
	
	// Material properties
	vec3 MaterialDiffuseColor  = texture( DiffuseTextureSampler, UV ).rgb;
	vec3 MaterialAmbientColor  = vec3(0.5, 0.5, 0.5) * MaterialDiffuseColor;
	vec3 MaterialSpecularColor = texture( SpecularTextureSampler, UV ).rgb * 0.3;

	// Local normal, in tangent space. V tex coordinate is inverted because normal map is in TGA (not in DDS) for better quality
	//vec3 TextureNormal_tangentspace = normalize(texture( NormalTextureSampler, vec2(UV.x,-UV.y) ).rgb*2.0 - 1.0);
	vec3 TextureNormal_tangentspace = normalize(texture( NormalTextureSampler, vec2(UV.x, -UV.y) ).rgb*2.0);
	
	// Distance to the light
	float distance = length(lightPos-worldPos);
	distance = (distance * distance);

	// Normal of the computed fragment, in camera space
	vec3 n = TextureNormal_tangentspace;
	// Direction of the light (from the fragment to the light)
	vec3 l = normalize(LightDirection_tangentspace);
	// Cosine of the angle between the normal and the light direction, 
	// clamped above 0
	//  - light is at the vertical of the triangle -> 1
	//  - light is perpendicular to the triangle -> 0
	//  - light is behind the triangle -> 0
	float cosTheta = clamp( dot( n,l ), 0,1 );

	// Eye vector (towards the camera)
	vec3 E = normalize(EyeDirection_tangentspace);
	// Direction in which the triangle reflects the light
	vec3 R = reflect(-l,n);
	// Cosine of the angle between the Eye vector and the Reflect vector,
	// clamped to 0
	//  - Looking into the reflection -> 1
	//  - Looking elsewhere -> < 1
	float cosAlpha = clamp( dot( E,R ), 0,1 );


//	color = MaterialAmbientColor + MaterialDiffuseColor * LightColor * LightPower * cosTheta / distance;

color = (MaterialAmbientColor + 
			MaterialDiffuseColor * LightColor * LightPower * cosTheta / distance + 
			MaterialSpecularColor * LightColor * LightPower * pow(cosAlpha,5) / distance);
return;//
	color = 
		// Ambient : simulates indirect lighting
		MaterialAmbientColor +
		// Diffuse : "color" of the object
		MaterialDiffuseColor * LightColor * LightPower * cosTheta / distance +
		// Specular : reflective highlight, like a mirror
		MaterialSpecularColor * LightColor * LightPower * pow(cosAlpha,5) / distance;
}