// Std. Includes
#include <string>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// GL includes
#include "Shader.h"

// Data structure
#include "Edge.h"
#include "Point.h"

// GLM Mathemtics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// For saving points
#include <vector>

using namespace std;

// Define the const number
#define MAX_ARRAY_NUMBER 100
#define WINDOW_HEIGHT 600
#define WINDOW_WIDTH 800

// Properties
GLuint screenWidth = WINDOW_WIDTH, screenHeight = WINDOW_HEIGHT;

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
//void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void PolygonFill();
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



vector<Point> vertices_Polygon;

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

	GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "Polygon scan-line fill", nullptr, nullptr); // Windowed
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
	Shader ourShader("vertex.vs", "fragment.frag");



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
    
	GLboolean done = false;
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
		//std::cout<<"while:" << xCurPos << "," << yCurPos << std::endl;

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
		else if (vertexCount >= 2 && LastVertex == true)
		{
			// Paint
			glBindVertexArray(VAO);
			glDrawArrays(GL_LINE_LOOP, 0, vertexCount);
			glBindVertexArray(0);
			
			// Fill the polygon
			
			// Fill the vector of vertex of the polyghon
			for (int i = 0; i < vertexCount; i++)
				vertices_Polygon.push_back(Point(vertices[3 * i], vertices[3 * i + 1]));
			
			/*cout << vertices_Polygon[0].getXInt();*/
			// Coumpute the maxium y position of vertices
			GLint maxYPos = 0;
			for (int i = 0; i<vertices_Polygon.size(); i++)
			{
				if (vertices_Polygon[i].getYInt() > maxYPos)
				{
					maxYPos = vertices_Polygon[i].getYInt();
				}
			}
			
			// Initialize ET & AET
			Edge *ET[WINDOW_HEIGHT];
			Edge *AET;
			
			for (int i = 0; i<maxYPos; i++)
			{
				ET[i] = new Edge();
				ET[i]->next = nullptr;
			}
			AET = new Edge();
			AET->next = nullptr;

			// Build the ET
			for (int i = 0; i<vertices_Polygon.size(); i++)
			{
				// Point 0,1 for processing,Point 2,3 for computing the Singular point of a curve 
				int x0 = vertices_Polygon[(i - 1 + vertices_Polygon.size()) % vertices_Polygon.size()].getXInt();
				int x1 = vertices_Polygon[i].getXInt();
				int x2 = vertices_Polygon[(i + 1) % vertices_Polygon.size()].getXInt();
				int x3 = vertices_Polygon[(i + 2) % vertices_Polygon.size()].getXInt();
				int y0 = vertices_Polygon[(i - 1 + vertices_Polygon.size()) % vertices_Polygon.size()].getYInt();
				int y1 = vertices_Polygon[i].getYInt();
				int y2 = vertices_Polygon[(i + 1) % vertices_Polygon.size()].getYInt();
				int y3 = vertices_Polygon[(i + 2) % vertices_Polygon.size()].getYInt();
				// drop the horizion line
				if (y1 == y2)
					continue;	
				int ymin = y1 > y2 ? y2 : y1;
				int ymax = y1 > y2 ? y1 : y2;
				float x = y1 > y2 ? x2 : x1;
				float dx = (x1 - x2) * 1.0f / (y1 - y2);
				// processing the Singular point
				if (((y1 < y2) && (y1 > y0)) || ((y2 < y1) && (y2 > y3)))
				{
					ymin++;
					x += dx;
				}
				// Build the new edge,insert into ET
				Edge *p = new Edge();
				p->ypos_top = ymax;
				p->xpos_down = x;
				p->back_slope = dx;
				p->next = ET[ymin]->next;
				ET[ymin]->next = p;
			}

			// from top to down 
			for (int i = 0; i<maxYPos; i++)
			{
				// take out all edges in ET to AET
				while (ET[i]->next)
				{
					//take out the first edge int ET which is in the current scan line 
					
					Edge *pInsert = ET[i]->next;
					Edge *p = AET;
					//search the appropriate position to insert node
					while (p->next)
					{
						if (pInsert->xpos_down > p->next->xpos_down)
						{
							p = p->next;
							continue;
						}
						if (pInsert->xpos_down == p->next->xpos_down && pInsert->back_slope > p->next->back_slope)
						{
							p = p->next;
							continue;
						}
						//find the position
						break;
					}
					// Delete the pInsert in ET,and insert it into AET
					ET[i]->next = pInsert->next;
					pInsert->next = p->next;
					p->next = pInsert;
				}

				// Pair the edge in AET and paint
				Edge *p = AET;
				while (p->next && p->next->next)
				{
					/*for (int x = p->next->xpos_down; x < p->next->next->xpos_down; x++)
					{
						glVertex2i(x, i);
					}*/

					GLfloat xPosTmp1 = (p->next->xpos_down - 400.0) / 400.0;
					GLfloat xPosTmp2 = (p->next->next->xpos_down - 400.0) / 400.0;

					GLfloat yPosTmp = (300.0 - i) / 300.0;
				    // Paint 
					vertices[vertexCount *3+ 0] = xPosTmp1;
					vertices[vertexCount * 3 +1] = yPosTmp;
					vertices[vertexCount * 3 +3] = xPosTmp2;
					vertices[vertexCount * 3 +4] = yPosTmp;

					// Update the buffer
					glBindBuffer(GL_ARRAY_BUFFER, VBO);
					glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
					glBindBuffer(GL_ARRAY_BUFFER, 0);

					// Paint
					glBindVertexArray(VAO);
					glDrawArrays(GL_LINE_STRIP, vertexCount, 2);
					glBindVertexArray(0);


					p = p->next->next;
				}

				// Delete the edge of y==ymax in AET
				p = AET;
				while (p->next)
				{
					if (p->next->ypos_top == i)
					{
						Edge *pDelete = p->next;
						p->next = pDelete->next;
						pDelete->next = nullptr;
						delete pDelete;
					}
					else
					{
						p = p->next;
					}
				}
				// Update xpos_down in AET,enter the next loop
				p = AET;
				while (p->next)
				{
					p->next->xpos_down += p->next->back_slope;
					p = p->next;
				}
			}
			if (!done)
				glfwSwapBuffers(window);
			done = true;

		}
		// Swap the buffers
		if(!done)
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
	xCurPos = (xpos - 400) / 400;
	yCurPos = (300 - ypos) / 300;
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
		if (vertexCount >= 3) LastVertex = true;
	}
}
