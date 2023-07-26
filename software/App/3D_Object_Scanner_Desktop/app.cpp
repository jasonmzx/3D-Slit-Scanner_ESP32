
// This Project was built by: Jason Manarroo

//C++ Built-in Libraries:
#include<iostream>
#include <sstream>
#include <thread>  // std::thread

// OpenGL & Related Libraries
#include<glad/glad.h>
#include<GLFW/glfw3.h>

	// GLM Imports for Matrix Calculations with OpenGL
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include <glm/ext.hpp>

// OpenCV & Image Processing Libraries
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

void tcp_server_main();

// Project Core Imports:
#include "pipeline.h"
#include "io_handle.h"
#include "camera_calibration.h"
#include "console_printing.h"


//	OpenGL Classes Imports:
#include "shaderClass.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include "Camera.h"

// CLI COMMANDS

struct RunTcpCommand {
	bool isPortSet;
	int port;
};

struct RenderCommand {
	std::string directory;

	bool isMidpointSet;
	int midpoint;

	bool isCutoffSet;
	int cutoff;

	bool isPipelineSet;
	std::string pipeline;
};

struct CameraCalibrationCommand {
	std::string directory;
};

//CLI helpers

bool is_integer(const std::string& s) {
	return !s.empty() && std::all_of(s.begin(), s.end(), ::isdigit);
}

//Constants

const unsigned int width = 1000;
const unsigned int height = 1000;

//Thread Variables 
//std::thread TCP_server_thread = std::thread(tcp_server_main);


int spawnOpenGL(VerticeObject payload) {

	//Loading in the Processed Object into an OpenGL Window to be displayed
	GLfloat* vertices = payload.vertices;
	GLuint* indices = payload.indices;

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

	int vertices_size = payload.vertices_length;
	GLsizeiptr indices_size = payload.indices_length;

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

void programCredit() {
	std::string text_decor = " #################### ";
	std::string r_p = " -> ";

	std::cout << yellow << text_decor << "Lazer Scanner / Profiler into 3D Project - DESKTOP APP - " << text_decor << reset << std::endl;
	std::cout << r_p << "Run TCP Server | rtcp [INT port]" << std::endl;
	std::cout << r_p << "Render Dataset | render <STR path/to/dataset> [INT midpoint] [INT cutoff] [STR pipeline]\n" << std::endl;

	std::cout << "Project by: jasonmzx , hope you enjoyed :-)\n\n\n" << std::endl;
}

int main() {

	//Roll the credits at beginning
	programCredit();

	while (true) {
		std::cout << "Enter command: ";
		std::string command;
		getline(std::cin, command);
		std::istringstream iss(command);
		std::vector<std::string> tokens{ std::istream_iterator<std::string>{iss},
										 std::istream_iterator<std::string>{} };

		if (!tokens.empty()) {

		// ! COMMAND EXECUTIONS HERE : 		
	
			if (tokens[0] == "rt" || tokens[0] == "rtcp") {
				RunTcpCommand runTcpCommand;
				runTcpCommand.isPortSet = false;

				if (tokens.size() > 1 && is_integer(tokens[1])) {
					runTcpCommand.port = std::stoi(tokens[1]);
					runTcpCommand.isPortSet = true;
				}
				// Print command data or handle runTcpCommand
				if (runTcpCommand.isPortSet) {
					std::cout << "TCP command activated. Port: " << runTcpCommand.port << "\n";
				}
				else {
					std::cout << "TCP command activated. No port specified.\n";
					tcp_server_main();
				}
			}
			else if (tokens[0] == "r" || tokens[0] == "render") {
				if (tokens.size() < 2) {
					std::string e = "Render command requires a directory path."; //TODO: swithc this to config path
					print_error(e);
				}
				else {
					RenderCommand renderCommand;
					renderCommand.directory = tokens[1];
					renderCommand.isMidpointSet = false;
					renderCommand.isCutoffSet = false;
					renderCommand.isPipelineSet = false;

					if (tokens.size() > 2 && is_integer(tokens[2])) {
						renderCommand.midpoint = std::stoi(tokens[2]);
						renderCommand.isMidpointSet = true;
					}
					if (tokens.size() > 3 && is_integer(tokens[3])) {
						renderCommand.cutoff = std::stoi(tokens[3]);
						renderCommand.isCutoffSet = true;
					}
					if (tokens.size() > 4) {
						renderCommand.pipeline = tokens[4];
						renderCommand.isPipelineSet = true;
					}
					// Print command data or handle renderCommand
					std::cout << "Render command activated. Directory: " << renderCommand.directory;


					// At this point, it's OK

					if (renderCommand.isMidpointSet) {
						std::cout << ", Midpoint: " << renderCommand.midpoint;
					}
					if (renderCommand.isCutoffSet) {
						std::cout << ", Cutoff: " << renderCommand.cutoff;
					}
					if (renderCommand.isPipelineSet) {
						std::cout << ", Pipeline: " << renderCommand.pipeline;
						
						VerticeObject pipeline_response = executePipeline(
							renderCommand.pipeline, 
							renderCommand.directory,
							renderCommand.midpoint,
							renderCommand.cutoff
						);

						int ogl_inst = spawnOpenGL(pipeline_response);
						return 0;
					}
					std::cout << "\n";
				}
			}
			else if (tokens[0] == "cc" || tokens[0] == "camera-calib") {
				
				//Assert for Correct Command Size:
				if (tokens.size() < 2) { 
					std::string e ="Camera Calibration Command requires a directory path.";
					print_error(e);
				}
				else {

					CameraCalibrationCommand CCComand;
					CCComand.directory = tokens[1];

					calibrate_camera_from_directory(CCComand.directory, 11, 7, true);
				}
				

			}
			else if (tokens[0] == "mc" || tokens[0] == "mkcfg" || tokens[0] == "mkconf") {
				// Ensure we have enough arguments for mandatory fields
				if (tokens.size() < 6) {
					std::string e = "Making of Dataset Config File requires more arguments...";
					print_error(e);
				}
				else {
					DatasetConfig configCommand;
					
					//Creation of Dataset Config
					
					configCommand.directory = tokens[1];
					configCommand.dataset_title = tokens[2];
					configCommand.step_angle_interval = std::stof(tokens[3]);
					configCommand.adjustment_per_angle = std::stof(tokens[4]);
					configCommand.relative_lazer_angle = std::stof(tokens[5]);
					configCommand.pixel_midpoint_x = std::stoi(tokens[6]);

					// Optional arguments
					if (tokens.size() > 7 && is_integer(tokens[7])) {
						configCommand.top_cutoff = std::stoi(tokens[7]);
					}
					if (tokens.size() > 8 && is_integer(tokens[8])) {
						configCommand.bottom_cutoff = std::stoi(tokens[8]);
					}

					// CREATION OF FILE:

					//Get filename
					std::string cfname;
					std::cout << green << "Enter a filename for your Config File : <config name>.cfg" << reset << std::endl;
					std::cin >> cfname;

					//! Check if USER enters a config name with file extension already, If so, don't add extension to str.
					// The string is either too short to contain ".cfg", or it does not end with ".cfg" therefore add it
					if (cfname.length() < 4 || cfname.rfind(".cfg") != cfname.length() - 4) {cfname += ".cfg";}

					WriteConfigToFile(configCommand, cfname, std::string("configs"));

					// Print command data
					//TODO: print nicely
					std::cout << "\n";
				}
			}

			//TODO: Implement `h` , `help` command

			else {
			std::string unknown_err = "Unknown Command : `" + tokens[0] + "` ... use the `h` or `help` command for help.";
			print_error(unknown_err);
			}
		}
	}
	return 0;
}