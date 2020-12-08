#version 330 core
layout(location = 0) out vec4 FragColor;

in vec3 worldPos;

uniform sampler2D renderTexture;


void main(){
	// FragColor = texture(renderTexture,TexCoords);
	FragColor = vec4(1,0,0,1);
}