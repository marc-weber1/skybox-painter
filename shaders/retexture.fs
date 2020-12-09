#version 330 core
layout(location = 0) out vec4 FragColor;

in vec4 clipPos;
in vec2 texCoords;

uniform sampler2D renderTexture;
// uniform sampler2D brushTexture;

uniform vec2 previousPoint;
uniform vec2 currentPoint;


void main(){
	
	// For debugging the z buffer, not sure wtf is happening in there honestly
	// FragColor = vec4(0.5*clipPos.z+0.5, 0, 0, 1);
	
	FragColor = texture( renderTexture, texCoords );
	
	if( 0.1 < clipPos.z ){ // Make sure it's not getting clipped by the near plane
		if( length( currentPoint - clipPos.xy ) < 0.2 ){
			FragColor = vec4(1,0,0,1);
		}
	}
}