
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

//Project imports
#include "image_processing.h"

//	process_img1();
#include "shaderClass.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"


const unsigned int width = 800;
const unsigned int height = 800;


// Vertices coordinates
//GLfloat vertices[] =
//{ //     COORDINATES     /        COLORS      /   TexCoord  //
//	-0.5f, 0.0f,  0.5f,     1.0f, 0.0f, 0.0f,	0.0f, 0.0f,
//	-0.5f, 0.0f, -0.5f,     0.0f, 1.0f, 0.0f,	5.0f, 0.0f,
//	 0.5f, 0.0f, -0.5f,     0.0f, 0.0f, 1.0f,	0.0f, 0.0f,
//	 0.5f, 0.0f,  0.5f,     1.0f, 1.0f, 0.0f,	5.0f, 0.0f,
//	 0.0f, 0.8f,  0.0f,     1.0f, 0.41f, 0.71f,	2.5f, 5.0f,
//};

//GLfloat vertices2[] =
//{ //     COORDINATES     /        COLORS      /   TexCoord  //
//	-0.5f, 0.0f, -0.5f,     1, 0 , 0,
//	-0.5f, 0.5f, -0.5f,     0, 1 , 0,	
//	 0.0f, 0.0f, -0.0f,     0, 0,  1
//};

//GLfloat vertices[] =
//{ 0, 0, 0, 1, 0, 0, 0.182576, -0.36608, 0.312326, 0, 1, 0, 0.178869, -0.364226, 0.308619, 0, 1, 0 };

VerticeObject load = gen();


GLfloat* vertices = load.vertices;

// Indices for vertices order (1x GLuint is 4 byte)
GLuint indices[] =
{
	0, 1, 2,
	0, 2, 3,
	0, 3, 4,
	0, 4, 5,
	0, 5, 6,
	0, 6, 7,
	0, 7, 8,
	0, 8, 9,
	0, 9, 10,
	0, 10, 11,
	0, 11, 12,
	0, 12, 13,
	0, 13, 14,
	0, 14, 15,
	0, 15, 16,
	0, 16, 17,
	0, 17, 18,
	0, 18, 19,
	0, 19, 20,
	0, 20, 21,
	0, 21, 22,
	0, 22, 23,
	0, 23, 24,
	0, 24, 25,
	0, 25, 26,
	0, 26, 27,
	0, 27, 28,
	0, 28, 29,
	0, 29, 30,
	0, 30, 31,
	0, 31, 32,
	0, 32, 33,
	0, 33, 34,
	0, 34, 35,
	0, 35, 36,
	0, 36, 37,
	0, 37, 38,
	0, 38, 39,
	0, 39, 40,
	0, 40, 41,
	0, 41, 42,
	0, 42, 43,
	0, 43, 44,
	0, 44, 45,
	0, 45, 46,
	0, 46, 47,
	0, 47, 48,
	0, 48, 49,
	0, 49, 50,
	0, 50, 51,
	0, 51, 52,
	0, 52, 53,
	0, 53, 54,
	0, 54, 55,
	0, 55, 56,
	0, 56, 57,
	0, 57, 58,
	0, 58, 59,
	0, 59, 60,
	0, 60, 61,
	0, 61, 62,
	0, 62, 63,
	0, 63, 64,
	0, 64, 65,
	0, 65, 66,
	0, 66, 67,
	0, 67, 68,
	0, 68, 69,
	0, 69, 70,
	0, 70, 71,
	0, 71, 72,
	0, 72, 73,
	0, 73, 74,
	0, 74, 75,
	0, 75, 76,
	0, 76, 77,
	0, 77, 78,
	0, 78, 79,
	0, 79, 80,
	0, 80, 81,
	0, 81, 82,
	0, 82, 83,
	0, 83, 84,
	0, 84, 85,
	0, 85, 86,
	0, 86, 87,
	0, 87, 88,
	0, 88, 89,
	0, 89, 90,
	0, 90, 91,
	 0, 91, 92,
	0, 92, 93,
	0, 93, 94,
	0, 94, 95,
	0, 95, 96,
	0, 96, 97,
	0, 97, 98,
	0, 98, 99,
	0, 99, 100,
	0, 100, 101,
	0, 101, 102,
	0, 102, 103,
	0, 103, 104,
	0, 104, 105,
	0, 105, 106,
	0, 106, 107,
	0, 107, 108,
	0, 108, 109,
	0, 109, 110,
	0, 110, 111,
	0, 111, 112,
	0, 112, 113,
	0, 113, 114,
	0, 114, 115,
	0, 115, 116,
	0, 116, 117,
	0, 117, 118,
	0, 118, 119,
	0, 119, 120,
	0, 120, 121,
	0, 121, 122,
	0, 122, 123,
	0, 123, 124,
	0, 124, 125,
	0, 125, 126,
	0, 126, 127,
	0, 127, 128,
	0, 128, 129,
	0, 129, 130,
	0, 130, 131
};

//GLuint* indices = new GLuint[3];



int main()
{

	std::cout << "Vertice Data: " << std::endl;

	for (int i = 0; i < 18; i++) {
		std::cout << vertices[i] << ", ";
	}
	std::cout << std::endl;

	//process_img1();
	// 
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
	GLFWwindow* window = glfwCreateWindow(width, height, "YoutubeOpenGL", NULL, NULL);
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


	int vertices_size = 72*load.vertices_length; //1 Vertex is 72 bytes, here I'm allocating space for 5
	int indices_size = sizeof(indices);


	std::cout << "Vertice Size: " << vertices_size << "\n Indices Size: "<< indices_size << std::endl;

	// Generates Vertex Buffer Object and links it to vertices
	VBO VBO1(vertices, vertices_size);
	// Generates Element Buffer Object and links it to indices
	EBO EBO1(indices, indices_size);

	// Links VBO attributes such as coordinates and colors to VAO
	VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, 6 * sizeof(float), (void*)0 );
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

	// Main while loop
	while (!glfwWindowShouldClose(window))
	{
		// Specify the color of the background
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		// Clean the back buffer and depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// Tell OpenGL which Shader Program we want to use
		shaderProgram.Activate();

		// Simple timer
		double crntTime = glfwGetTime();
		if (crntTime - prevTime >= 1 / 60)
		{
			rotation += 0.005f; //rotation speed
			prevTime = crntTime;
		}

		// Initializes matrices so they are not the null matrix
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 proj = glm::mat4(1.0f);

		// Assigns different transformations to each matrix
		model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 1.0f, 0.0f));
		view = glm::translate(view, glm::vec3(0.0f, -0.5f, -2.0f));
		proj = glm::perspective(glm::radians(45.0f), (float)width / height, 0.1f, 100.0f);

		// Outputs the matrices into the Vertex Shader
		int modelLoc = glGetUniformLocation(shaderProgram.ID, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		int viewLoc = glGetUniformLocation(shaderProgram.ID, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		int projLoc = glGetUniformLocation(shaderProgram.ID, "proj");
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));

		// Assigns a value to the uniform; NOTE: Must always be done after activating the Shader Program
		glUniform1f(uniID, 0.5f);
		// Binds texture so that is appears in rendering
		// Bind the VAO so OpenGL knows to use it
		VAO1.Bind();
		// Draw primitives, number of indices, datatype of indices, index of indices
		glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(int), GL_UNSIGNED_INT, 0);
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