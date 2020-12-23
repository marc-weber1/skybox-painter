#version 330 core
layout(location = 0) out vec4 FragColor;

in vec4 clipPos; // from -1 to 1
in vec2 texCoords;

uniform sampler2D renderTexture;
uniform sampler2D brushTexture;

uniform vec2 previousPoint; // [-1,1]^2
uniform vec2 currentPoint; // [-1,1]^2

uniform vec4 brushColour;
uniform float brushSize;

void main(){
	
	// For debugging the z buffer, not sure wtf is happening in there honestly
	// FragColor = vec4(0.5*clipPos.z+0.5, 0, 0, 1);
	
	FragColor = texture( renderTexture, texCoords );

	
	
	if( 0.1 < clipPos.z ){ // Make sure it's not getting clipped by the near plane
		if( abs(currentPoint.x - clipPos.x) < brushSize && abs(currentPoint.y - clipPos.y) < brushSize){ 
			// Figure out the point on the brush texture to look at
			vec2 position = (clipPos.xy - currentPoint)/brushSize;
			position.x = (position.x + 1.0) / 2.0;
			position.y = (position.y + 1.0) / 2.0;

			float brushAlpha = texture(brushTexture, position).a;
			vec3 blendedColour = (1-brushAlpha) * texture(renderTexture, texCoords).rgb + brushAlpha * brushColour.rgb;

			FragColor = vec4(blendedColour, 1);
		}
	}
}