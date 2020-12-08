#version 330 core

//Inputs (set through VAO's and VBO's in C++)
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

//Outputs (sent to fragment shader)
out vec3 worldPos;

void main(){
	worldPos = aPos;
	
	gl_Position = vec4(2*aTexCoords-vec2(1,1),0,1);
}