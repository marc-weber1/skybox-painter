#pragma once

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


class TextureCube{
public:
	TextureCube(const char* file_path){
		stbi_set_flip_vertically_on_load(true);
		data = stbi_load(file_path, &width, &height, &nrChannels, 0);
		//if(!data) return false;
	}
	
	~TextureCube(){
		stbi_image_free(data);
	}
	
	void setActiveTexture(){
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	}
	
private:
	int width, height, nrChannels;
	unsigned char* data;
};