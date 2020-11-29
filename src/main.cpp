/**
 *	SKYBOX PAINTER
 *	
 */

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define GLM_FORCE_CTOR_INIT
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <memory>
#include <vector>
#include <string>

#include "LookAtCamera.h" //Replace?
#include "Shader.h"
#include "SkyboxCube.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"


// CONSTANTS
const int SCR_HEIGHT = 600;
const int SCR_WIDTH = 800;
const double ROTATE_SPEED_X = -500; // Change the sign to invert the axis
const double ROTATE_SPEED_Y = -300; // Change the sign to invert the axis

// GLOBAL VARIABLES
LookAtCamera camera(glm::vec3(0.f,0.f,0.f),0.1f);
std::unique_ptr<SkyboxCube> skybox;
std::vector<TextureCube> skybox_textures;
std::unique_ptr<Shader> current_shader;
double lastMouseX = 0.;
double lastMouseY = 0.;
bool should_redraw = false;

int brush = 0;
glm::vec4 brushColour = glm::vec4(1, 1, 1, 1);

// Timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;


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

// ------ GUI FUNCTIONS ------
void Import() {
	std::cout << "Import\n";
}

void Export() {
	std::cout << "Export\n";
}



// ------ RENDERING ------

// Simple helper function to load an image into a OpenGL texture with common settings
bool LoadTextureFromFile(const char* filename, GLuint* out_texture, int* out_width, int* out_height)
{
	// Load from file
	int image_width = 0;
	int image_height = 0;
	unsigned char* image_data = stbi_load(filename, &image_width, &image_height, NULL, 4);
	if (image_data == NULL)
		return false;

	// Create a OpenGL texture identifier
	GLuint image_texture;
	glGenTextures(1, &image_texture);
	glBindTexture(GL_TEXTURE_2D, image_texture);

	// Setup filtering parameters for display
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same

	// Upload pixels into texture
#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
	stbi_image_free(image_data);

	*out_texture = image_texture;
	*out_width = image_width;
	*out_height = image_height;

	return true;
}

void redraw_display(GLFWwindow* window){
	
	// Update camera angle
	glm::mat4 VP = glm::perspective(camera.getFOV(), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f)*camera.getViewMatrix();
	current_shader->setMat4("VP",VP);
	
	// DEBUG WIREFRAME RENDER
	/*glClear(GL_COLOR_BUFFER_BIT);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glLineWidth(3.0);*/
	
	skybox->drawVertices();
	
	// Present frame
	// glfwSwapBuffers(window);

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
	
	
	// Load Shader
	current_shader.reset(new Shader("shaders/default.vs","shaders/oneimage.fs"));
	current_shader->use();
	
	// Load Image
	skybox_textures.push_back( TextureCube("E:/Users/facade/Documents/Github/skybox-painter/model-converter/skyboxcube_testbox.png") );
	current_shader->setTexture( &skybox_textures[0], 0 );
	
	// Initialize the skybox, including buffering it as a mesh to GPU
	skybox.reset(new SkyboxCube());

	// Initialize ImGui
	// The following setup code is taken from https://blog.conan.io/2019/06/26/An-introduction-to-the-Dear-ImGui-library.html
	const char* glsl_version = "#version 130";
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);
	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Set up images previews for brushes
	int numBrushes = 3;
	std::vector<GLuint> brushTextures;
	std::vector<const char*> filenames{ "../brushes/brush-0.png", "../brushes/brush-1.png", "../brushes/brush-2.png" };
	for (int i = 0; i < numBrushes; i++) {
		int brush_image_width = 0;
		int brush_image_height = 0;
		GLuint brush_image_texture = 0;
		bool ret = LoadTextureFromFile(filenames[i], &brush_image_texture, &brush_image_width, &brush_image_height);
		brushTextures.push_back(brush_image_texture);
		std::cout << ret << std::endl;
	}
	
	should_redraw = true; // Draw the first frame
	
	bool continue_program = true;
	while(continue_program){
		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		//Check input
		glfwPollEvents();

		// Clear screen
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		//Check for exit
		if(glfwGetKey(window,GLFW_KEY_ESCAPE)==GLFW_PRESS || 
			glfwWindowShouldClose(window)!=0){
			continue_program=false;
			should_redraw=false;
		}

		// Feed inputs to ImGui, start new frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// Create GUI
		ImGui::Begin("File");
		{
			// Import/Export
			if (ImGui::Button("Import")) {
				Import();
			}
			ImGui::SameLine();
			if (ImGui::Button("Export")) {
				Export();
			}
		}
		ImGui::End();

		// TODO: Figure out whether RadioButton with images or ImageButton works better
		ImGui::Begin("Brushes");
		{
			for (int i = 0; i < brushTextures.size(); i++) {
				ImGui::SameLine();
				std::string name = "Brush " + std::to_string(i);
				ImGui::RadioButton(name.c_str(), &brush, i); 
				
			}
			ImGui::NewLine();
			for (int i = 0; i < brushTextures.size(); i++) {
				ImGui::SameLine(0, 16);
				ImVec4 border;
				if (brush == i) {
					border = ImVec4(0.26, 0.96, 0.26, 1);
				}
				else {
					border = ImVec4(0, 0, 0, 0);
				}
				ImGui::Image((void*)(intptr_t)brushTextures[i], ImVec2(64, 64), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1), border);
			}
			ImGui::NewLine();
			ImGui::ColorEdit4("Brush Colour", glm::value_ptr(brushColour));
		}
		ImGui::End();
		
		//Draw frame
		if(should_redraw){
			redraw_display(window);
			// This is set to true to allow ImGui to work as intended
			should_redraw=true;
		}

		// Render ImGui into screen
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glfwSwapBuffers(window);
	}
	
	glfwTerminate();
	
	for(int i=0; i<skybox_textures.size(); i++){
		skybox_textures[i].free();
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	return 0;
}