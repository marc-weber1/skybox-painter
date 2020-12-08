#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <iostream>
#include <filesystem>

// #include <stb_image.h>

#include "SkyboxCube.h"


class TextureCube{
public:
	TextureCube(const char* file_path){
		glGenTextures(1, &tex);
		
		stbi_set_flip_vertically_on_load(true);
		unsigned char* data = stbi_load(file_path, &width, &height, &nrChannels, 0);
		//std::cout << (int) data[0] << (int) data[1] << (int) data[2] << (int) data[3] << std::endl;
		if(!data){
			std::cerr << "Could not load texture " << file_path << std::endl;
		}
		
		glBindTexture(GL_TEXTURE_2D, tex);
		//v Account for the number of channels ? Assuming 4
		// Also errorcheck so that data is freed no matter what
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		
		//glGenerateMipmap(GL_TEXTURE_2D); //Should not be necessary
		
		free(data);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		
		//glBindTexture(GL_TEXTURE_2D,0); //Reset
	}
	
	void setRenderTarget(){
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, tex, 0);
	}
	
	void setActiveTexture(){
		glBindTexture(GL_TEXTURE_2D, tex);
	}
	
	void drawRect(glm::vec2 topLeft, glm::vec2 topRight, glm::vec2 bottomLeft, glm::vec2 bottomRight){ // Draw a rectangle of red as a test
		
	}
	
	int width, height, nrChannels;
	
private:
	GLuint tex;
	
	
};