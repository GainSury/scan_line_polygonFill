// Std. Includes
#include <string>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// GL includes
#include "Shader.h"

// GLM Mathemtics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Other Libs
#include <SOIL.h>

// Define the max array number
#define MAX_ARRAY_NUMBER 100

// Properties
GLuint screenWidth = 800, screenHeight = 600;

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
//void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
//void Do_Movement();

// Global variable

// Record the key state
GLint keys[1024];
// Record the current x,y position(the normalized coordinate)
GLfloat xCurPos = 0;
GLfloat yCurPos = 0;
GLint vertexCount = 0;
// Set up our vertex data (and buffer(s)) and attribute pointers
GLfloat vertices[3 * 100] = { 0 };
GLboolean LastVertex = false;

// The MAIN function, from here we start our application and run our Game loop
int main()
{
	// Init GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "LearnOpenGL", nullptr, nullptr); // Windowed
	glfwMakeContextCurrent(window);

	// Set the required callback functions
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	//glfwSetScrollCallback(window, scroll_callback);

	//// Options
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Initialize GLEW to setup the OpenGL Function pointers
	glewExperimental = GL_TRUE;
	glewInit();

	// Define the viewport dimensions
	glViewport(0, 0, screenWidth, screenHeight);

	// Setup some OpenGL options
	glEnable(GL_DEPTH_TEST);

	// Setup and compile our shaders
	Shader ourShader("06test.vs", "06test.frag");

	

	GLuint VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	// Bind our Vertex Array Object first, then bind and set our buffers and pointers.
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// Prepare the buffer to recieve the data
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 3 * MAX_ARRAY_NUMBER, nullptr, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	

	glBindVertexArray(0); // Unbind VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbind VBO

	// Paint loop
	while (!glfwWindowShouldClose(window))
	{
		// Check and call events
		glfwPollEvents();
		
		// Clear the colorbuffer
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Activate the shader
		ourShader.Use();
	/*	std::cout<<"while:" << xCurPos << "," << yCurPos << std::endl;
		
		vertices[0] = 0.5f;
		vertices[1] = 0.5f;

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

			glBindVertexArray(VAO);
			glDrawArrays(GL_POINTS, 0, 1);
			glBindVertexArray(0);*/


		// Update the buffer
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// Paint the first vertex
		if (vertexCount == 1)
		{
			glBindVertexArray(VAO);
			glDrawArrays(GL_POINTS, 0, vertexCount);
			glBindVertexArray(0);
		}
		
		// Paint the uncompleted polygon 
		if (vertexCount >= 2 && LastVertex == false)
		{
			//paint
			glBindVertexArray(VAO);
			glDrawArrays(GL_LINE_STRIP, 0, vertexCount);
			glBindVertexArray(0);
		}
		//  Complete the polygon
		else if(vertexCount >= 2 && LastVertex == true)
		{
			//paint
			glBindVertexArray(VAO);
			glDrawArrays(GL_LINE_LOOP, 0, vertexCount);
			glBindVertexArray(0);
		}


		// Swap the buffers
		glfwSwapBuffers(window);
	}
	// Properly de-allocate all resources once they've outlived their purpose
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glfwTerminate();
	return 0;
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
			keys[key] = true;
		else if (action == GLFW_RELEASE)
			keys[key] = false;
	}
}

//
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	xCurPos = (xpos - 400) /400;
	yCurPos = (300 - ypos)/300;
	//std::cout << xCurPos << "," << yCurPos << std::endl;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && LastVertex == false)
	{
		//std::cout << "Left" << std::endl;
		// Record the current vertex position
		vertices[vertexCount * 3] = xCurPos;
		vertices[vertexCount * 3 + 1] = yCurPos;
		vertexCount++;
	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS && LastVertex == false)
	{
		if(vertexCount >= 3) LastVertex = true;
	}
}
//
//
//void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
//{
//	camera.ProcessMouseScroll(yoffset);
//}