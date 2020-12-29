/*
 *  TextureCube
 *    Represents a texture that can map onto a cube, with the mipmap defined by SkyboxCube
 *    Aspect ratio should be anything where the height is bigger than the width
*/


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
	TextureCube() : width(1500), height(2000), nrChannels(4){
		glGenTextures(1, &tex);
		
		glBindTexture(GL_TEXTURE_2D, tex);
		// v Initialize with a transparent black image
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
		
		// No filtering, so the image isn't blurred
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}
	
	void freeTexture(){
		glDeleteTextures( 1, &tex );
	}
	
	// Import from a file into GPU memory
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
	
	// Save whatever the GPU has in memory into a file
	void exportCubemap(const char* filepath){
		const unsigned int NUM_CHANNELS = 4;
		
		size_t image_size = width*height*NUM_CHANNELS;
		void* ram_buffer = malloc( image_size );
		
		glBindTexture(GL_TEXTURE_2D, tex);
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, ram_buffer);
		
		stbi_flip_vertically_on_write(true);
		stbi_write_png(filepath, width, height, NUM_CHANNELS, ram_buffer, width*NUM_CHANNELS );
		
		free(ram_buffer);
		
	}
	
	// Set this texture as the render target, call before redrawing to the texture
	void setRenderTarget(){
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, tex, 0);
	}
	
	// Set this texture as active, call before using the texture in a shader
	void setActiveTexture(){
		glBindTexture(GL_TEXTURE_2D, tex);
	}
	
	int width, height, nrChannels;
	
private:
	GLuint tex; // The texture we're storing in GPU memory
	
};