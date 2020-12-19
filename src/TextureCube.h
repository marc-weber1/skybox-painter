#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <iostream>
#include <filesystem>
#include <cstdlib>

// #include <stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include "SkyboxCube.h"


class TextureCube{
public:
	TextureCube() : width(1), height(1), nrChannels(4){
		glGenTextures(1, &tex);
		
		glBindTexture(GL_TEXTURE_2D, tex);
		// v Initialize with a transparent black image
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		
		//glBindTexture(GL_TEXTURE_2D,0); //Reset
	}
	
	void freeTexture(){
		glDeleteTextures( 1, &tex );
	}
	
	bool import(const char* file_path){
		
		stbi_set_flip_vertically_on_load(true);
		unsigned char* data = stbi_load(file_path, &width, &height, &nrChannels, 0);
		//std::cout << (int) data[0] << (int) data[1] << (int) data[2] << (int) data[3] << std::endl;
		if(!data){
			std::cout << "Failed to load texture." << std::endl;
			return false;
		}
		
		glBindTexture(GL_TEXTURE_2D, tex);
		//v Account for the number of channels ? Assuming 4
		// Also errorcheck so that data is freed no matter what
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		
		free(data);
		
		//glBindTexture(GL_TEXTURE_2D,0); //Reset
		return true;
	}
	
	void exportCubemap(const char* filepath){
		// Edit filepath so it has a png at the end?
		const unsigned int NUM_CHANNELS = 4;
		
		size_t image_size = width*height*NUM_CHANNELS;
		void* ram_buffer = malloc( image_size );
		
		glBindTexture(GL_TEXTURE_2D, tex);
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, ram_buffer);
		
		stbi_flip_vertically_on_write(true);
		stbi_write_png(filepath, width, height, NUM_CHANNELS, ram_buffer, width*NUM_CHANNELS );
		
		free(ram_buffer);
		
	}
	
	void setRenderTarget(){
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, tex, 0);
	}
	
	void setActiveTexture(){
		glBindTexture(GL_TEXTURE_2D, tex);
	}
	
	int width, height, nrChannels;
	
private:
	GLuint tex;
	
};