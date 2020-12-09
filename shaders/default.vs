#version 330 core

//Inputs (set through VAO's and VBO's in C++)
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

//Outputs (sent to fragment shader)
out vec2 TexCoords;
out vec3 Normal;

//Constants (set directly in C++)
uniform mat4 VP;

void main(){
	TexCoords = aTexCoords;
	Normal = aNormal;
	
	gl_Position = VP * vec4(aPos,1);
}