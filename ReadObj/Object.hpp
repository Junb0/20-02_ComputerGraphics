#pragma once
#include <GL/glew.h>
#include <GL/freeglut.h> 
#include <GL/freeglut_ext.h>
#include <glm/glm/glm.hpp>
#include <glm/glm/ext.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <iostream>

#include "ReadObj.h"

class Object {
	GLuint vao;
	GLuint vbo[3];
	GLuint ebo;


};