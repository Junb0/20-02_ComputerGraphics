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
public:
	GLuint vao;
	GLuint vbo[3];
	GLuint ebo;

	float* vPosData;
	float* vNormalData;
	float* vTextureCoordinateData;
	int* indexData;
	int vertexCount;
	int indexCount;

	const char* fileName;


	Object(const char* fileName) {
		this->fileName = fileName;
	}

	void InitObj() {
		ReadObj(this->fileName, this->vPosData, this->vNormalData, this->vTextureCoordinateData, this->indexData, this->vertexCount, this->indexCount);
	}
};