/*******************************************************************
//This code is created for learning purposes and for a university project
Author: Lovisa Hassler
Date: 2016

Tutorials, sources and helpful links:

Joey de Vries for getting started with OpenGL, assembling shaders, camera class, basic lighting etc.
http://www.learnopengl.com/ 

ThinMatrix on YouTube for OpenGL water tutorial (specifically using DuDv-mapping and normal mapping).
https://www.youtube.com/user/ThinMatrix/ 

Stefan Gustavson for being a great professor as well as created great sources to learn about simplex noise
http://staffwww.itn.liu.se/~stegu/simplexnoise/ 

*********************************************************************/



#include <iostream>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

//other libs
#include <SOIL/SOIL.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Other includes
#include "Shader.h"
#include "Camera.h"


// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void MoveCamera();
GLuint loadTexture(GLchar* path);

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;

// Camera
glm::vec3 cameraPos(0.3f, 0.8f, 2.0f);
Camera camera(cameraPos);

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

//mode swapping bool
bool normalMapping = true;


// The MAIN function, from here we start the application and run the game loop
int main()
{
	// Init GLFW
	glfwInit();
	// Set all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Water", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Set the required callback functions and mouse capture
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glEnable(GL_CULL_FACE);
	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	glewInit();

	// Define the viewport dimensions
	glViewport(0, 0, WIDTH, HEIGHT);
	glEnable(GL_DEPTH_TEST);

	//environment parameters
	glm::vec3 lightPos(0.3f, 1.0f, 0.0f);

	// Build and compile our shader program
	Shader waterShader("watershader.vertex", "watershader.fragment");
	Shader lampShader("lampshader.vertex", "lampshader.fragment");

	// load textures for mapping
	GLuint normalMap = loadTexture("textures/normalmap2.png");
	glUniform1i(glGetUniformLocation(waterShader.Program, "normalMap"), 0);
	GLuint dudvMap = loadTexture("textures/dudv.png");
	glUniform1i(glGetUniformLocation(waterShader.Program, "dudvMap"), 1);


	// Set up vertex data (and buffer(s)) and attribute pointers
	GLfloat waterVertices[] = {
		// Positions          // normals           // Texture Coords
		0.5f,  0.5f, 0.0f,    0.0f, 0.0f, 1.0f,   1.0f, 1.0f, // Top Right
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,// Bottom left
		0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,    1.0f, 0.0f,// Bottom right
		-0.5f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f,    0.0f, 1.0f// Top Left 
	};

	GLuint indices[] = {  // Note that we start from 0!
		0, 1, 2, // First Triangle
		0, 3, 1  // Second Triangle
	};

	GLfloat lightBoxVertices[] = {
		-1.0f,-1.0f,-1.0f, // triangle 1 : begin
		-1.0f,-1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f, // triangle 1 : end
		1.0f, 1.0f,-1.0f, // triangle 2 : begin
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f,-1.0f, // triangle 2 : end
		1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f,-1.0f,
		1.0f,-1.0f,-1.0f,
		1.0f, 1.0f,-1.0f,
		1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f,-1.0f,
		1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f,-1.0f, 1.0f,
		1.0f,-1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f,-1.0f,-1.0f,
		1.0f, 1.0f,-1.0f,
		1.0f,-1.0f,-1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f,-1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f,-1.0f,
		-1.0f, 1.0f,-1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f,-1.0f, 1.0f
	};
	

	GLuint waterVBO, waterVAO, waterEBO;
	glGenVertexArrays(1, &waterVAO);
	glGenBuffers(1, &waterVBO);
	glGenBuffers(1, &waterEBO);
	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(waterVAO);

	glBindBuffer(GL_ARRAY_BUFFER, waterVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(waterVertices), waterVertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, waterEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// Normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	//texture coordinate attribute 
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);

	glBindVertexArray(0); // Unbind VAO

	GLuint lightVAO, lightVBO;
	glGenVertexArrays(1, &lightVAO);
	glGenBuffers(1, &lightVBO);

	glBindVertexArray(lightVAO);

	glBindBuffer(GL_ARRAY_BUFFER, lightVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(lightBoxVertices), lightBoxVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	float waveFactor = 0.0f;
	const float WAVE_SPEED = 0.03f;


	// rendering loop
	while (!glfwWindowShouldClose(window))
	{
		// Set frame time
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();
		MoveCamera();

		// Render
		// Clear the buffers
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//animate light
		lightPos.x = sin(glfwGetTime()) * 0.5f ;
		lightPos.z = -0.5 + cos(glfwGetTime()) * 0.7f;
		//lightPos.y = 0.5f + sin(glfwGetTime()*2.0f)/2.0f;

		//variables used for the normalmapping shading
		waveFactor += WAVE_SPEED * deltaTime;
		if (waveFactor >= 1.0)
			waveFactor = 0.0f;


		//activate watershader
		waterShader.Use();

		glm::mat4 view;
		view = camera.GetViewMatrix();
		glm::mat4 projection = glm::perspective(camera.Zoom, float(WIDTH) / float(HEIGHT), 0.1f, 100.0f);
		glm::mat4 model;
		model = glm::translate(model, glm::vec3(0.3f, 0.0f, -0.5f)); //translate from origin for noise to work better
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)); //rotate around x-axis
		model = glm::scale(model, glm::vec3(2.0));
		
		//GLint lightPosLoc = glGetUniformLocation(lightShader.Program, "lightPos");
		GLint viewPosLoc = glGetUniformLocation(waterShader.Program, "viewPos");
		
		glUniform1i(glGetUniformLocation(waterShader.Program, "normalMapping"), normalMapping);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, normalMap);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, dudvMap);

		GLint matAmbientLoc = glGetUniformLocation(waterShader.Program, "material.ambient");
		GLint matDiffuseLoc = glGetUniformLocation(waterShader.Program, "material.diffuse");
		GLint matSpecularLoc = glGetUniformLocation(waterShader.Program, "material.specular");
		GLint matShineLoc = glGetUniformLocation(waterShader.Program, "material.shininess");

		glUniform3f(matAmbientLoc, 0.0f, 0.5f, 0.8f);
		glUniform3f(matDiffuseLoc, 0.0f, 0.5f, 0.8f);
		glUniform3f(matSpecularLoc, 0.6f, 0.6f, 0.6f);
		glUniform1f(matShineLoc, 10.0f);
		
		GLint moveFactorLoc = glGetUniformLocation(waterShader.Program, "moveFactor");
		glUniform1f(moveFactorLoc, waveFactor);
		GLint timeLoc = glGetUniformLocation(waterShader.Program, "time");
		glUniform1f(timeLoc, currentFrame);


		//glUniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z);
		glUniform3f(viewPosLoc, camera.Position.x, camera.Position.y, camera.Position.z);

		GLint lightPositionLoc = glGetUniformLocation(waterShader.Program, "light.position");

		GLint lightAmbientLoc = glGetUniformLocation(waterShader.Program, "light.ambient");
		GLint lightDiffuseLoc = glGetUniformLocation(waterShader.Program, "light.diffuse");
		GLint lightSpecularLoc = glGetUniformLocation(waterShader.Program, "light.specular");

		glUniform3f(lightAmbientLoc, 0.2f, 0.2f, 0.2f);
		glUniform3f(lightDiffuseLoc, 0.8f, 0.8f, 0.8f); // Let's darken the light a bit to fit the scene
		glUniform3f(lightSpecularLoc, 1.0f, 1.0f, 1.0f);
		glUniform3f(lightPositionLoc, lightPos.x, lightPos.y, lightPos.z);



		// Get the uniform locations
		GLint modelLoc = glGetUniformLocation(waterShader.Program, "model");
		GLint viewLoc = glGetUniformLocation(waterShader.Program, "view");
		GLint projLoc = glGetUniformLocation(waterShader.Program, "projection");
		// Pass the matrices to the shader
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		glBindVertexArray(waterVAO);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//glDrawArrays(GL_TRIANGLES, 0, 36);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);


		lampShader.Use();

		model = glm::mat4(); //reset
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.05f)); // Make it a smaller cube

		modelLoc = glGetUniformLocation(lampShader.Program, "model");
		viewLoc = glGetUniformLocation(lampShader.Program, "view");
		projLoc = glGetUniformLocation(lampShader.Program, "projection");
		// Set matrices
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		// Draw the light object (using light's vertex attributes)
		glBindVertexArray(lightVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glBindVertexArray(0); // unbind
		glUseProgram(0);

		// Swap the screen buffers
		glfwSwapBuffers(window);
	}
	// Properly de-allocate all resources once they've outlived their purpose
	glDeleteVertexArrays(1, &waterVAO);
	glDeleteBuffers(1, &waterVBO);
	glDeleteBuffers(1, &waterEBO);

	glDeleteVertexArrays(1, &lightVAO);
	glDeleteBuffers(1, &lightVBO);


	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
	return 0;
}

GLuint loadTexture(GLchar* path)
{
	//Generate texture ID and load texture data 
	GLuint textureID;
	glGenTextures(1, &textureID);
	int width, height;
	unsigned char* image = SOIL_load_image(path, &width, &height, 0, SOIL_LOAD_RGB);
	// Assign texture to ID
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	// Parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	SOIL_free_image_data(image);
	return textureID;
}


// Moves/alters the camera positions based on user input

#pragma region "User input"
bool keys[1024];
bool keysPressed[1024];
bool firstMouse = true;
GLfloat lastX = WIDTH / 2, lastY = HEIGHT / 2;

void MoveCamera()
{
	// Camera controls
	if (keys[GLFW_KEY_W])
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (keys[GLFW_KEY_S])
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (keys[GLFW_KEY_A])
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (keys[GLFW_KEY_D])
		camera.ProcessKeyboard(RIGHT, deltaTime);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	if (keys[GLFW_KEY_SPACE] && !keysPressed[GLFW_KEY_SPACE])
	{
		normalMapping = false;
		keysPressed[GLFW_KEY_SPACE] = true;
	}
	if (!keys[GLFW_KEY_SPACE] && keysPressed[GLFW_KEY_SPACE])// && !keysPressed[GLFW_KEY_SPACE])
	{
		normalMapping = true;
		keysPressed[GLFW_KEY_SPACE] = false;
	}

	if (keys[GLFW_KEY_Z])
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	/*
	if (keys[GLFW_KEY_SPACE] && keysPressed[GLFW_KEY_SPACE])
	{
		normalMapping = !normalMapping;
		keysPressed[GLFW_KEY_SPACE] = false;
	}*/
}
// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	//cout << key << endl;
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
		{
			keys[key] = true;
		}
		else if (action == GLFW_RELEASE)
			keys[key] = false;
	}

}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	GLfloat xoffset = xpos - lastX;
	GLfloat yoffset = lastY - ypos;  // Reversed since y-coordinates go from bottom to left

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset/6);
}
