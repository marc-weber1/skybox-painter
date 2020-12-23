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

#include <cmath>
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
#include "imfilebrowser.h"


// CONSTANTS
const int SCR_HEIGHT = 600;
const int SCR_WIDTH = 800;
const double ROTATE_SPEED_X = -500; // Change the sign to invert the axis
const double ROTATE_SPEED_Y = -300; // Change the sign to invert the axis
const GLenum DRAW_BUFFERS[1] = {GL_COLOR_ATTACHMENT0};
const double FRAMES_PER_SECOND = 60.;

// GLOBAL VARIABLES
LookAtCamera camera(glm::vec3(0.f,0.f,0.f),0.1f);
std::unique_ptr<SkyboxCube> skybox;
std::vector<TextureCube> skybox_textures;
int current_texture = -1; // Should be a number from 0 to skybox_textures.size()-1, or -1 if no texture is selected
std::unique_ptr<Shader> texture_shader;
std::unique_ptr<Shader> current_shader;
double lastMouseX = 0.;
double lastMouseY = 0.;
//bool should_redraw = true; //Should we redraw the frame?
bool should_redraw_texture = false;
glm::vec2 previous_brush_point;
glm::vec2 current_brush_point;
GLuint texture_framebuffer = 0;

int brush = 0;
std::vector<GLuint> brushTextures; 
glm::vec4 brushColour = glm::vec4(1, 0, 0, 1);
float brushSize = 0.2;

// Timing
double lastFrame = 0.0;


// ------ INPUT HANDLING FUNCTIONS ------

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
	//should_redraw = true;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods){
	
	// Make sure the mouse isnt clicking on a UI window
	if( !ImGui::GetIO().WantCaptureMouse){
	
		if(button==GLFW_MOUSE_BUTTON_RIGHT && action==GLFW_PRESS){
			// Begin rotating
			glfwGetCursorPos(window,&lastMouseX,&lastMouseY);
		}
		
		else if(button==GLFW_MOUSE_BUTTON_LEFT && action==GLFW_PRESS){
			int width, height;
			glfwGetWindowSize(window, &width, &height);
			double xpos, ypos;
			glfwGetCursorPos(window,&xpos,&ypos);
			
			// Begin Drawing
			current_brush_point = glm::vec2( 2.*xpos/width-1., -2.*ypos/height+1 );
			should_redraw_texture = true;
		}
		
		else if(button==GLFW_MOUSE_BUTTON_LEFT && action==GLFW_RELEASE){
			// Release the previous point so the shader doesn't get confused
			previous_brush_point = glm::vec2(nan(""),nan("")); // Use nans to signal to the gpu not to check the previous brush point
		}
		
	}
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos){
	
	// Make sure the mouse isnt clicking on a UI window
	if( !ImGui::GetIO().WantCaptureMouse){
	
		int width, height;
		glfwGetWindowSize(window, &width, &height);
		
		//Handle rotations
		if(glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_RIGHT)==GLFW_PRESS){
			
			// Rotate around, accounting for the window size
			camera.rotateCamera(
				(GLfloat) ( ROTATE_SPEED_X/width*(lastMouseX-xpos) ),
				(GLfloat) ( ROTATE_SPEED_Y/height*(lastMouseY-ypos) )
			);
			
			//should_redraw=true;
		}
		
		//Handle drawing
		if(glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_LEFT)==GLFW_PRESS){
			current_brush_point = glm::vec2( 2.*xpos/width-1., -2.*ypos/height+1 );
			should_redraw_texture = true;
		}
		
		lastMouseX=xpos;
		lastMouseY=ypos;
		
	}
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
	
	// Bind the correct framebuffer in case we're on one that renders to image right now
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// And fix the screen size
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	glViewport(0, 0, width, height);
	
	// Make sure we're on the right shader
	current_shader->use();
	
	// Set the correct textures
	skybox_textures[0].setActiveTexture();
	
	// Update camera angle
	glm::mat4 VP = glm::perspective(camera.getFOV(), 1.f * width / height, 0.1f, 10.f) * camera.getViewMatrix();
	current_shader->setMat4("VP",VP);
	
	// Update optional shader uniforms
	current_shader->setFloat("_Time", (float) glfwGetTime());
	
	// DEBUG WIREFRAME RENDER
	/*glClear(GL_COLOR_BUFFER_BIT);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glLineWidth(3.0);*/
	
	skybox->drawVertices();

}

void redraw_texture(TextureCube* tex, GLfloat window_aspect_ratio){
	glBindFramebuffer(GL_FRAMEBUFFER, texture_framebuffer);
	glViewport(0, 0, tex->width, tex->height);
	
	texture_shader->use();
	
	glActiveTexture(GL_TEXTURE0);
	skybox_textures[0].setActiveTexture(); //Necessary? probably
	texture_shader->setInt("renderTexture", 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, brushTextures[brush]);
	texture_shader->setInt("brushTexture", 1);
	
	// Set the matrix so it can rasterize-check instead of raycast
	glm::mat4 VP = glm::perspective(camera.getFOV(), window_aspect_ratio, 0.1f, 10.f) * camera.getViewMatrix();
	current_shader->setMat4("VP",VP);

	// Set uniforms
	texture_shader->setVec2("previousPoint",previous_brush_point);
	texture_shader->setVec2("currentPoint",current_brush_point);
	texture_shader->setVec4("brushColour", brushColour);
	texture_shader->setFloat("brushSize", brushSize);
	
	skybox->drawVertices();
	
	previous_brush_point = current_brush_point;
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
	
	// Load texturing Shader
	texture_shader.reset(new Shader("shaders/retexture.vs","shaders/retexture.fs"));
	previous_brush_point = glm::vec2(nan(""),nan("")); // Use nans to signal to the gpu not to check the previous brush point
	
	// Load Rendering Shader
	current_shader.reset(new Shader("shaders/default.vs","shaders/oneimage.fs"));
	
	// Load Image
	skybox_textures.push_back( TextureCube() );
	current_texture = 0;
	
	// Set up framebuffer to render to (TURNS THE IMAGE BLACK?)
	glGenFramebuffers(1, &texture_framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, texture_framebuffer);
	skybox_textures[current_texture].setRenderTarget();
	glDrawBuffers(1, DRAW_BUFFERS);
	
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
		std::cerr << "Could not initialize image framebuffer;" << std::endl;
		std::cerr << glGetError() << std::endl;
		return -1;
	}
	
	// Initialize the skybox, including buffering it as a mesh to GPU
	skybox.reset(new SkyboxCube());

	// Initialize ImGui
	// The following setup code is taken from https://blog.conan.io/2019/06/26/An-introduction-to-the-Dear-ImGui-library.html
	const char* glsl_version = "#version 330 core";
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
	std::vector<const char*> filenames{ "brushes/brush-0.png", "brushes/brush-1.png", "brushes/brush-2.png" };
	for (int i = 0; i < numBrushes; i++) {
		int brush_image_width = 0;
		int brush_image_height = 0;
		GLuint brush_image_texture = 0;
		bool ret = LoadTextureFromFile(filenames[i], &brush_image_texture, &brush_image_width, &brush_image_height);
		brushTextures.push_back(brush_image_texture);
	}
	
	// Set up file dialogs
	ImGui::FileBrowser importImage;
	importImage.SetTitle("Import Image");
	importImage.SetTypeFilters({".png",".jpg",".bmp",".hdr",".gif",".pic"});
	ImGui::FileBrowser exportImage(ImGuiFileBrowserFlags_EnterNewFilename);
	exportImage.SetTitle("Export Image");
	// exportImage.SetTypeFilters({".png",".jpg",".bmp",".hdr",".gif",".pic"});
	
	bool continue_program = true;
	while(continue_program){ // Main Event Loop
		
		//Check input
		glfwPollEvents();
		
		//Check for exit
		if(glfwGetKey(window,GLFW_KEY_ESCAPE)==GLFW_PRESS || 
			glfwWindowShouldClose(window)!=0){
			continue_program=false;
		}
		
		
		// per-frame time logic
		// --------------------
		double currentFrame = glfwGetTime();
		double deltaTime = currentFrame - lastFrame;
		
		if(continue_program && deltaTime >= 1./FRAMES_PER_SECOND){ // Time imgui so the FPS is capped at some maximum
		
			// Starting a new frame, reset the timer
			lastFrame = currentFrame;
			
			// If the texture needs to be redrawn, do that first (Maybe move this to its own timer/thread?)
			if(should_redraw_texture && current_texture >= 0 && current_texture < skybox_textures.size()){
				int width, height;
				glfwGetWindowSize(window, &width, &height);
				
				redraw_texture( &skybox_textures[current_texture], 1.f*width/height );
				should_redraw_texture = false;
			}

			// Feed inputs to ImGui, start new frame
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();


			// Create Menus
			ImGui::Begin("File");
			{
				// Import/Export
				if (ImGui::Button("Import Image")) {
					importImage.Open();
				}
				ImGui::SameLine();
				if (ImGui::Button("Export Image")) {
					exportImage.Open();
				}
			}
			ImGui::End();
			
			//File pickers
			importImage.Display();
			if(importImage.HasSelected()){
				// std::cout << "Importing image from: " << importImage.GetSelected().generic_string() << std::endl;
				skybox_textures[0].import( (char*) importImage.GetSelected().generic_string().c_str() );
				importImage.ClearSelected();
			}
			exportImage.Display();
			if(exportImage.HasSelected()){
				std::cout << "Saved texture to: " << exportImage.GetSelected().generic_string().c_str() << std::endl;
				// Edit filepath so it always has a png at the end? Right now people have to type it in themself
				skybox_textures[0].exportCubemap( exportImage.GetSelected().generic_string().c_str() );
				exportImage.ClearSelected();
			}

			// TODO: Figure out whether RadioButton with images or ImageButton works better
			ImGui::Begin("Brushes");
			{
				for (int i = 0; i < brushTextures.size(); i++) {
					ImGui::SameLine();
					std::string name = "Brush " + std::to_string(i);
					ImGui::RadioButton(name.c_str(), &brush, i);;
					
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
				ImGui::DragFloat("Brush Size", &brushSize, 0.01, 0.01, 1.0);
			}
			ImGui::End();
			
			
			// First render the skybox
			redraw_display(window);

			// Then render ImGui into screen
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
			
			glfwSwapBuffers(window);
		}
		
	}
	
	glfwTerminate();

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	return 0;
}