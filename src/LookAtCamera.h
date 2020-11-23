#pragma once

#include <iostream>

#include <glad/glad.h>

#ifndef GLM_FORCE_CTOR_INIT
#define GLM_FORCE_CTOR_INIT
#endif
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

const GLfloat TWO_PI = 6.28318531f;

class LookAtCamera{
public:

	// Breaks if up = <0,0,a>
	LookAtCamera(glm::vec3 target = glm::vec3(0.f, 0.f, 0.f), GLfloat distance = 3.f, glm::vec3 up = glm::vec3(0.f, 1.f, 0.f), GLfloat phi = 0.f, GLfloat theta = TWO_PI/4, GLfloat fov = TWO_PI/8, GLfloat rotateSpeed = 0.002f, GLfloat zoomSpeed = 0.5f)
			: target(target), up(up), phi(phi), theta(theta), r(distance), fov(fov), rotateSpeed(rotateSpeed), zoomSpeed(zoomSpeed) {
		right = glm::normalize(glm::cross(up,glm::vec3(0.f,0.f,-1.f)));
	}
	
	GLfloat getFOV(){ //in radians
		return fov;
	}
	
	glm::vec3 getViewPos(){
		return glm::rotate(glm::mat4(),phi,up) * glm::rotate(glm::mat4(),theta,right) * glm::vec4(up,1)*r;
	}
	
	glm::mat4 getViewMatrix(){
		return glm::lookAt(getViewPos(),target,up);
	}
	
	void rotateCamera(GLfloat dphi,GLfloat dtheta){
		
		phi += dphi*TWO_PI*rotateSpeed;
		theta += dtheta*TWO_PI*rotateSpeed;
		
		if(theta >= TWO_PI/2) theta = TWO_PI/2 - 0.001f;
		else if(theta <= 0) theta = 0.001f;
	}
	
	void zoomCamera(GLfloat dr){
		r += dr*zoomSpeed;
		
		if(r<0) r = 0.001f;
	}
	
private:
	
	glm::vec3 target, up, right;
	GLfloat phi, theta, r;
	GLfloat fov, rotateSpeed, zoomSpeed;
	
};