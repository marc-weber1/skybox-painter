#pragma once

#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


class TextureCube{
public:
	TextureCube(const char* file_path){
		stbi_set_flip_vertically_on_load(true);
		data = stbi_load(file_path, &width, &height, &nrChannels, 0);
		if(!data){
			std::cerr << "Could not load texture " << file_path << std::endl;
		}
	}
	
	void free(){
		stbi_image_free(data);
	}
	
	void setActiveTexture(){
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	
private:
	int width, height, nrChannels;
	unsigned char* data;
};