#version 330 core

//Inputs (set through VAO's and VBO's in C++)
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

//Outputs (sent to fragment shader)
out vec4 clipPos;
out vec2 texCoords;

uniform mat4 VP;


void main(){
	clipPos = VP*vec4(aPos,1);
	
	gl_Position = vec4(2*aTexCoords-vec2(1,1),0,1);
	texCoords = aTexCoords;
}