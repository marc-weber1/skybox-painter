/**
 *	SKYBOX PAINTER
 *	
 */


#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define GLM_FORCE_CTOR_INIT
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <memory>

#include "LookAtCamera.h" //Replace?
#include "Shader.h"
#include "skyboxshapes.h"


// CONSTANTS
const int SCR_HEIGHT = 600;
const int SCR_WIDTH = 800;

// GLOBAL VARIABLES
LookAtCamera camera(glm::vec3(0.f,0.f,0.f),0.1f);
std::unique_ptr<Shader> current_shader;
GLuint VertexBufferID, VertexVAOID;
double lastMouseX = 0.;
double lastMouseY = 0.;
bool should_redraw = false;




// ------ INPUT HANDLING FUNCTIONS ------

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods){
	if(button==GLFW_MOUSE_BUTTON_LEFT && action==GLFW_PRESS){
		// Begin rotating
		glfwGetCursorPos(window,&lastMouseX,&lastMouseY);
	}
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos){
	if(glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_1)==GLFW_PRESS){
		// Rotate around
		
		camera.rotateCamera((GLfloat) (lastMouseX-xpos),(GLfloat) (lastMouseY-ypos));
		
		lastMouseX=xpos;
		lastMouseY=ypos;
		
		should_redraw=true;
	}
}



// ------ RENDERING ------

void redraw_display(GLFWwindow* window){
	
	// Update camera angle
	glm::mat4 VP = glm::perspective(camera.getFOV(), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f)*camera.getViewMatrix();
	current_shader->setMat4("VP",VP);
	
	// DEBUG WIREFRAME RENDER
	glClear(GL_COLOR_BUFFER_BIT);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glLineWidth(3.0);
	
	glDrawArrays( GL_TRIANGLES, 0, sizeof(SKYBOX_CUBE)/sizeof(SkyboxVertex) );
	
	// Present frame
	glfwSwapBuffers(window);
}



// ------ INITIALIZATION / MAIN LOOP ------
int main(){
	// glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "CPSC 591/691 A2", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window,cursor_position_callback);
    //glfwSetScrollCallback(window, scroll_callback);
	glfwSetMouseButtonCallback(window,mouse_button_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
	
	
	//Configure buffer
	glGenBuffers(1,&VertexBufferID);
	glBindBuffer(GL_ARRAY_BUFFER,VertexBufferID);
	
	//Generate VAO
	glGenVertexArrays(1,&VertexVAOID);
	glBindVertexArray(VertexVAOID);
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
	
	// Load shader
	current_shader.reset(new Shader("shaders/default.vs","shaders/solidwhite.fs"));
	current_shader->use();
	
	// Buffer model (just cube for now)
	glBufferData( GL_ARRAY_BUFFER, sizeof(SKYBOX_CUBE), SKYBOX_CUBE, GL_STATIC_DRAW );
	
	
	should_redraw = true; // Draw the first frame
	
	bool continue_program = true;
	while(continue_program){
		//Check input
		glfwPollEvents();
		
		//Check for exit
		if(glfwGetKey(window,GLFW_KEY_ESCAPE)==GLFW_PRESS || 
			glfwWindowShouldClose(window)!=0){
			continue_program=false;
			should_redraw=false;
		}
		
		//Draw frame
		if(should_redraw){
			redraw_display(window);
			should_redraw=false;
		}
	}
	
	glfwTerminate();
	return 0;
}