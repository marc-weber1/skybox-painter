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
#include "SkyboxCube.h"


// CONSTANTS
const int SCR_HEIGHT = 600;
const int SCR_WIDTH = 800;
const double ROTATE_SPEED_X = -500; // Change the sign to invert the axis
const double ROTATE_SPEED_Y = -300; // Change the sign to invert the axis

// GLOBAL VARIABLES
LookAtCamera camera(glm::vec3(0.f,0.f,0.f),0.1f);
std::unique_ptr<SkyboxCube> skybox;
std::unique_ptr<Shader> current_shader;
double lastMouseX = 0.;
double lastMouseY = 0.;
bool should_redraw = false;




// ------ INPUT HANDLING FUNCTIONS ------

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
	should_redraw=true;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods){
	if(button==GLFW_MOUSE_BUTTON_RIGHT && action==GLFW_PRESS){
		// Begin rotating
		glfwGetCursorPos(window,&lastMouseX,&lastMouseY);
	}
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos){
	
	//Handle rotations
	if(glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_RIGHT)==GLFW_PRESS){
		int width, height;
		glfwGetWindowSize(window, &width, &height);
		
		// Rotate around, accounting for the window size
		camera.rotateCamera(
			(GLfloat) ( ROTATE_SPEED_X/width*(lastMouseX-xpos) ),
			(GLfloat) ( ROTATE_SPEED_Y/height*(lastMouseY-ypos) )
		);
		
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
	
	skybox->drawVertices();
	
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
	
	
	// Load shader
	current_shader.reset(new Shader("shaders/default.vs","shaders/solidwhite.fs"));
	current_shader->use();
	
	// Initialize the skybox, including buffering it as a mesh to GPU
	skybox.reset(new SkyboxCube());
	
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