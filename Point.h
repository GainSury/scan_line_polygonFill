#ifndef POINT_H
#define POINT_H
// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

#define WINDOW_HEIGHT 600
#define WINDOW_WIDTH 800

class Point
{

public:
	// The width and height of the current window
	const static GLint height = WINDOW_HEIGHT;
	const static GLint width = WINDOW_WIDTH;
	GLfloat x;
	GLfloat y;
	Point(GLfloat x, GLfloat y)
	{
		this->x = x;
		this->y = y;
	}

	// Convert x,y position to windows size
	GLint getXInt()
	{
		GLint half_widhth = width / 2;
		return (GLint)(this->x * half_widhth + half_widhth);
	}

	GLint getYInt()
	{
		GLint half_height = height / 2;
		return (GLint)(half_height - this->y * half_height);
	}

	
};
#endif
