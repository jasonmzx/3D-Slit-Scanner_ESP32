#version 330 core

// Outputs colors in RGBA

out vec4 FragColor;


// Inputs the color from the Vertex Shader

in vec3 color;


// Inputs the texture coordinates from the Vertex Shader

// Gets the Texture Unit from the main function
//uniform sampler2D tex0;

uniform vec3 lightPos;
in vec3 normal;

void main()
{	

	const vec3 lightNormal = normalize(vec3(-1,-1,-1));	

	float scalar = max(0, dot(lightNormal, normal));
	FragColor = vec4(color * scalar, 1);
}