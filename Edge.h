#ifndef EDGE_H
#define EDGE_H
// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

class Edge
{
public:
	GLfloat xpos_down;
	GLfloat back_slope;
	GLfloat ypos_top;
	Edge* next;
};

#endif