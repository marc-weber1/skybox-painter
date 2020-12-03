#pragma once

class SkyboxCube{
public:

	struct UVBox{
		glm::vec2 corners[4]; //top left, top right, bottom left, bottom right
	};
	

	SkyboxCube(){
		//Generate VAO
		glGenVertexArrays(1,&VertexVAOID);
		glBindVertexArray(VertexVAOID);
		
		//Generate VBO
		glGenBuffers(1,&VertexBufferID);
		glBindBuffer( GL_ARRAY_BUFFER, VertexBufferID );
		
		//Where are the data chunks in the buffer?
		int stride = sizeof(SkyboxVertex);
		glVertexAttribPointer( //position
			0,3,GL_FLOAT,GL_FALSE,stride,(void*)0
		);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer( //normals
			1,3,GL_FLOAT,GL_FALSE,stride,(void*) sizeof(glm::vec3)
		);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer( //tex coords
			2,2,GL_FLOAT,GL_FALSE,stride,(void*) (2*sizeof(glm::vec3))
		);
		glEnableVertexAttribArray(2);
		
		bufferModel();
		
		//Unbind VAO for safety
		glBindVertexArray(0);
	}
	
	/*~SkyboxCube(){
		//Delete the VBO/VAO?
	}*/

	void bufferModel(){
		glBufferData( GL_ARRAY_BUFFER, sizeof(SKYBOX_CUBE), SKYBOX_CUBE, GL_STATIC_DRAW );
	}

	void drawVertices(){
		glBindVertexArray( VertexVAOID );
		
		glDrawArrays( GL_TRIANGLES, 0, sizeof(SKYBOX_CUBE)/sizeof(SkyboxVertex) );
		
		//Unbind VAO for safety
		glBindVertexArray(0);
	}
	
	void raycast(glm::vec3 direction, GLfloat radius, std::vector<UVBox>* uv_rects_out){
		
	}

	glm::vec2 getCubeSkyboxUV(glm::vec2 ray_dir){
		return glm::vec2();
	}

private:

	GLuint VertexBufferID, VertexVAOID;

	struct SkyboxVertex{
		glm::vec3 pos;
		glm::vec3 norm;
		glm::vec2 uv;
	};

	const SkyboxVertex SKYBOX_CUBE[36] = {
		{ {1.000000,1.000000,-1.000000},   {0.000000,-1.000000,0.000000},   {0.625000,0.749957}},
        { {1.000000,1.000000,1.000000},   {0.000000,-1.000000,0.000000},   {0.375043,0.749957}},
        { {-1.000000,1.000000,1.000000},   {0.000000,-1.000000,0.000000},   {0.375043,0.999913}},
        { {1.000000,1.000000,-1.000000},   {0.000000,-1.000000,0.000000},   {0.625000,0.749957}},
        { {-1.000000,1.000000,1.000000},   {0.000000,-1.000000,0.000000},   {0.375043,0.999913}},
        { {-1.000000,1.000000,-1.000000},   {0.000000,-1.000000,0.000000},   {0.625000,0.999913}},
        { {1.000000,-1.000000,1.000000},   {0.000000,0.000000,0.000000},   {0.375043,0.500000}},
        { {-1.000000,-1.000000,1.000000},   {0.000000,0.000000,0.000000},   {0.125087,0.500000}},
        { {-1.000000,1.000000,1.000000},   {0.000000,0.000000,0.000000},   {0.125087,0.749957}},
        { {1.000000,-1.000000,1.000000},   {0.000000,0.000000,0.000000},   {0.375043,0.500000}},
        { {-1.000000,1.000000,1.000000},   {0.000000,0.000000,0.000000},   {0.125087,0.749957}},
        { {1.000000,1.000000,1.000000},   {0.000000,0.000000,0.000000},   {0.375043,0.749957}},
        { {-1.000000,-1.000000,1.000000},   {1.000000,0.000000,0.000000},   {0.375043,0.250043}},
        { {-1.000000,-1.000000,-1.000000},   {1.000000,0.000000,0.000000},   {0.625000,0.250043}},
        { {-1.000000,1.000000,-1.000000},   {1.000000,0.000000,0.000000},   {0.625000,0.000087}},
        { {-1.000000,-1.000000,1.000000},   {1.000000,0.000000,0.000000},   {0.375043,0.250043}},
        { {-1.000000,1.000000,-1.000000},   {1.000000,0.000000,0.000000},   {0.625000,0.000087}},
        { {-1.000000,1.000000,1.000000},   {1.000000,0.000000,0.000000},   {0.375043,0.000087}},
        { {-1.000000,-1.000000,-1.000000},   {0.000000,1.000000,0.000000},   {0.625000,0.250043}},
        { {-1.000000,-1.000000,1.000000},   {0.000000,1.000000,0.000000},   {0.375043,0.250043}},
        { {1.000000,-1.000000,1.000000},   {0.000000,1.000000,0.000000},   {0.375043,0.500000}},
        { {-1.000000,-1.000000,-1.000000},   {0.000000,1.000000,0.000000},   {0.625000,0.250043}},
        { {1.000000,-1.000000,1.000000},   {0.000000,1.000000,0.000000},   {0.375043,0.500000}},
        { {1.000000,-1.000000,-1.000000},   {0.000000,1.000000,0.000000},   {0.625000,0.500000}},
        { {1.000000,-1.000000,-1.000000},   {-1.000000,0.000000,0.000000},   {0.625000,0.500000}},
        { {1.000000,-1.000000,1.000000},   {-1.000000,0.000000,0.000000},   {0.375043,0.500000}},
        { {1.000000,1.000000,1.000000},   {-1.000000,0.000000,0.000000},   {0.375043,0.749957}},
        { {1.000000,-1.000000,-1.000000},   {-1.000000,0.000000,0.000000},   {0.625000,0.500000}},
        { {1.000000,1.000000,1.000000},   {-1.000000,0.000000,0.000000},   {0.375043,0.749957}},
        { {1.000000,1.000000,-1.000000},   {-1.000000,0.000000,0.000000},   {0.625000,0.749957}},
        { {-1.000000,-1.000000,-1.000000},   {0.000000,0.000000,0.000000},   {0.874957,0.500000}},
        { {1.000000,-1.000000,-1.000000},   {0.000000,0.000000,0.000000},   {0.625000,0.500000}},
        { {1.000000,1.000000,-1.000000},   {0.000000,0.000000,0.000000},   {0.625000,0.749957}},
        { {-1.000000,-1.000000,-1.000000},   {0.000000,0.000000,0.000000},   {0.874957,0.500000}},
        { {1.000000,1.000000,-1.000000},   {0.000000,0.000000,0.000000},   {0.625000,0.749957}},
        { {-1.000000,1.000000,-1.000000},   {0.000000,0.000000,0.000000},   {0.874957,0.749957}}
	};
	
};