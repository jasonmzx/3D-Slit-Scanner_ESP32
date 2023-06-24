
// This Project was built by: Jason Manarroo

//C++ Built-in Libraries:
#include<iostream>
#include <thread>  // std::thread

// OpenGL & Related Libraries
#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include <glm/ext.hpp>

//TODO: Import & Get GLM working in the project

// OpenCV & Image Processing Libraries
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

void tcp_server_main();

//Project imports
#include "image_processing.h"

//	process_img1();
#include "shaderClass.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include "Camera.h"


const unsigned int width = 1000;
const unsigned int height = 1000;


//Thread Variables 
std::thread TCP_server_thread = std::thread(tcp_server_main);


// Vertices coordinates
//GLfloat vertices[] =
//{ //     COORDINATES     /        COLORS      /   TexCoord  //
//	-0.5f, 0.0f,  0.5f,     1.0f, 0.0f, 0.0f,	0.0f, 0.0f,
//	-0.5f, 0.0f, -0.5f,     0.0f, 1.0f, 0.0f,	5.0f, 0.0f,
//	 0.5f, 0.0f, -0.5f,     0.0f, 0.0f, 1.0f,	0.0f, 0.0f,
//	 0.5f, 0.0f,  0.5f,     1.0f, 1.0f, 0.0f,	5.0f, 0.0f,
//	 0.0f, 0.8f,  0.0f,     1.0f, 0.41f, 0.71f,	2.5f, 5.0f,
//};

VerticeObject load2 = gen2(); //Generates Vertices & Indices

VerticeObject load = gen(); //Generates Vertices & Indices

GLfloat* vertices = load.vertices;
GLuint* indices = load.indices; // 0, 1, 2 , 0, 2, 3, 0, 3 ,4 ....

//GLuint* indices = new GLuint[3];

int main()
{
	std::cout << "Indices (First 20): " << std::endl;


	for (int i = 0; i < 20; i++) {
		std::cout << indices[i] << ", ";
	}
	std::cout << std::endl;

	// Initialize GLFW

	glfwInit();

	// Tell GLFW what version of OpenGL we are using 
	// In this case we are using OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Tell GLFW we are using the CORE profile
	// So that means we only have the modern functions
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create a GLFWwindow object of 800 by 800 pixels, naming it "YoutubeOpenGL"
	GLFWwindow* window = glfwCreateWindow(width, height, "3D Object Scanner", NULL, NULL);
	// Error check if the window fails to create
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	// Introduce the window into the current context
	glfwMakeContextCurrent(window);

	//Load GLAD so it configures OpenGL


	gladLoadGL();


	// Specify the viewport of OpenGL in the Window
	// In this case the viewport goes from x = 0, y = 0, to x = 800, y = 800
	glViewport(0, 0, width, height);

	// Generates Shader object using shaders default.vert and default.frag
	Shader shaderProgram("default.vert", "default.frag");

	// Generates Vertex Array Object and binds it
	VAO VAO1;
	VAO1.Bind();

	int vertices_size = load.vertices_length;
	GLsizeiptr indices_size = load.indices_length;

	std::cout << "Vertice Size: " << vertices_size << "\n Indices Size: " << indices_size << std::endl;

	// Generates Vertex Buffer Object and links it to vertices
	VBO VBO1(vertices, vertices_size);
	// Generates Element Buffer Object and links it to indices
	EBO EBO1(indices, indices_size);

	// Links VBO attributes such as coordinates and colors to VAO
	VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, 6 * sizeof(float), (void*)0);
	VAO1.LinkAttrib(VBO1, 1, 3, GL_FLOAT, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	//VAO1.LinkAttrib(VBO1, 2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	// Unbind all to prevent accidentally modifying them
	VAO1.Unbind();
	VBO1.Unbind();
	EBO1.Unbind();

	// Gets ID of uniform called "scale"
	GLuint uniID = glGetUniformLocation(shaderProgram.ID, "scale");

	// Original code from the tutorial
	/*Texture brickTex("brick.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
	brickTex.texUnit(shaderProgram, "tex0", 0);*/

	// Variables that help the rotation of the pyramid
	float rotation = 0.05f;
	double prevTime = glfwGetTime();

	// Enables the Depth Buffer
	glEnable(GL_DEPTH_TEST);

	Camera camera(width, height, glm::vec3(0.0f, 0.0f, 2.0f));

	// Main while loop
	while (!glfwWindowShouldClose(window))
	{
		// Specify the color of the background
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		// Clean the back buffer and depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// Tell OpenGL which Shader Program we want to use
		shaderProgram.Activate();

		// Handles camera inputs
		camera.Inputs(window);
		// Updates and exports the camera matrix to the Vertex Shader
		camera.Matrix(45.0f, 0.1f, 100.0f, shaderProgram, "camMatrix");

		// Assigns a value to the uniform; NOTE: Must always be done after activating the Shader Program
		glUniform1f(uniID, 0.5f);
		// Binds texture so that is appears in rendering
		// Bind the VAO so OpenGL knows to use it
		VAO1.Bind();
		// Draw primitives, number of indices, datatype of indices, index of indices
		glDrawElements(GL_TRIANGLES, indices_size, GL_UNSIGNED_INT, 0);
		// Swap the back buffer with the front buffer
		glfwSwapBuffers(window);
		// Take care of all GLFW events
		glfwPollEvents();
	}

	// Delete all the objects we've created
	VAO1.Delete();
	VBO1.Delete();
	EBO1.Delete();
	shaderProgram.Delete();
	// Delete window before ending the program
	glfwDestroyWindow(window);
	// Terminate GLFW before ending the program
	glfwTerminate();
	return 0;
}