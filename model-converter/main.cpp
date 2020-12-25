
#include <stdio.h>

#include "model_obj.h"

int main(){
	ModelOBJ model;
	
	model.import("skyboxcube.obj");
	
	int indices = model.getNumberOfIndices();
	printf("%d\n",indices);
	const int* indexbuf = model.getIndexBuffer();
	for(int i=0; i<indices; i++){
		const ModelOBJ::Vertex& v = model.getVertex( indexbuf[i] );
		
		printf("\t{ {%f,%f,%f},   {%f,%f,%f},   {%f,%f}},\n", v.position[0], v.position[1], v.position[2], v.normal[0], v.normal[1], v.normal[2], v.texCoord[0], v.texCoord[1] );
	}
	
	
	/*int verts = model.getNumberOfVertices();
	for(int i=0; i<verts; i++){
		const ModelOBJ::Vertex& v = model.getVertex(i);
		printf("\t{ %f,%f,%f,   %f,%f,%f,   %f,%f},\n", v.position[0], v.position[1], v.position[2], v.normal[0], v.normal[1], v.normal[3], v.texCoord[0], v.texCoord[1] );
	}*/
}