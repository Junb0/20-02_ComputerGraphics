#include<stdlib.h>
#include<stdio.h>
#include<iostream>
#include<random>
#include<vector>
#include<gl/glew.h>
#include<gl/freeglut.h>
#include<gl/freeglut_ext.h>
#include<glm/glm/glm.hpp>
#include<glm/glm/ext.hpp>
#include<glm/glm/gtc/matrix_transform.hpp>
#include<glm/glm/gtc/type_ptr.hpp>
#include<math.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define UP 1
#define DOWN 2
#define LEFT 3
#define RIGHT 4

#define TRIHEIGHT 0.2f
#define TRIWIDTH 0.4f

#define BOXTOP 0.5f
#define BOXBOTTOM -0.5f
#define BOXLEFT -0.5f
#define BOXRIGHT 0.5f

#define XX(x) (float)(x - (width / 2)) / (float)width * 2.0f
#define YY(y) (-1.0f) * (float)(y - (height / 2)) / (float)height * 2.0f

#define v1 {0.0f, 0.0f, 0.0f}
#define v2 {0.0f, 0.0f, 1.0f}
#define v3 {0.0f, 1.0f, 0.0f}
#define v4 {0.0f, 1.0f, 1.0f}
#define v5 {1.0f, 0.0f, 0.0f}
#define v6 {1.0f, 0.0f, 1.0f}
#define v7 {1.0f, 1.0f, 0.0f}
#define v8 {1.0f, 1.0f, 1.0f}
#define v9 {0.5f, 1.0f, 0.5f}
#define rndclr {urdColor(dre), urdColor(dre), urdColor(dre)}
#define CLR(a) {clr[a - 1][0], clr[a - 1][1], clr[a - 1][2]}

using namespace std;

GLvoid drawScene();
GLvoid Reshape(int w, int h);
GLvoid Timer(int value);
GLvoid InitBuffer();
GLvoid InitShader();
GLvoid Timer(int value);
GLvoid Keyboard(unsigned char key, int x, int y);

GLuint VAO[4], VBO[4];

random_device rd;
default_random_engine dre(rd());
uniform_real_distribution<> urd1(-0.7f, 0.7f);
uniform_real_distribution<> urd2(0.005f, 0.02f);
uniform_real_distribution<> urdColor(0.0f, 1.0f);
GLfloat clr[6][3] = {
	rndclr,rndclr,rndclr,rndclr,rndclr,rndclr
};

char* filetobuf(const char* file) {
	FILE* fptr;
	long length;
	char* buf;

	fptr = fopen(file, "rb");
	if (!fptr)
		return NULL;
	fseek(fptr, 0, SEEK_END);
	length = ftell(fptr);
	buf = (char*)malloc(length + 1);
	fseek(fptr, 0, SEEK_SET);
	fread(buf, length, 1, fptr);
	fclose(fptr);
	buf[length] = 0;

	return buf;
}

GLfloat Cube[] = {	// 카메라 기준
	// 앞면
	-0.5f, -0.5f, 0.5f,		0.0, 1.0, 0.0,		0.0, 0.0,
	0.5f, -0.5f, 0.5f,		0.0, 1.0, 0.0,		1.0, 0.0,
	-0.5f, 0.5f, 0.5f,		0.0, 1.0, 0.0,		0.0, 1.0,

	-0.5f, 0.5f, 0.5f,		0.0, 1.0, 0.0,		0.0, 1.0,
	0.5f, -0.5f, 0.5f,		0.0, 1.0, 0.0,		1.0, 0.0,
	0.5f, 0.5f, 0.5f,		0.0, 1.0, 0.0,		1.0, 1.0,
	// 오른쪽 면
	0.5f, -0.5f, 0.5f,		0.0, 1.0, 0.0,		0.0, 0.0,
	0.5f, -0.5f, -0.5f,		0.0, 1.0, 0.0,		1.0, 0.0,
	0.5f, 0.5f, 0.5f,		0.0, 1.0, 0.0,		0.0, 1.0,

	0.5f, 0.5f, 0.5f,		0.0, 1.0, 0.0,		0.0, 1.0,
	0.5f, -0.5f, -0.5f,		0.0, 1.0, 0.0,		1.0, 0.0,
	0.5f, 0.5f, -0.5f,		0.0, 1.0, 0.0,		1.0, 1.0,
	// 뒷쪽 면
	0.5f, -0.5f, -0.5f,		0.0, 1.0, 0.0,		0.0, 0.0,
	-0.5f, -0.5f, -0.5f,	0.0, 1.0, 0.0,		1.0, 0.0,
	0.5f, 0.5f, -0.5f,		0.0, 1.0, 0.0,		0.0, 1.0,

	0.5f, 0.5f, -0.5f,		0.0, 1.0, 0.0,		0.0, 1.0,
	-0.5f, -0.5f, -0.5f,	0.0, 1.0, 0.0,		1.0, 0.0,
	-0.5f, 0.5f, -0.5f,		0.0, 1.0, 0.0,		1.0, 1.0,
	// 왼쪽 면
	-0.5f, -0.5f, -0.5f,	0.0, 1.0, 0.0,		0.0, 0.0,
	-0.5f, -0.5f, 0.5f,		0.0, 1.0, 0.0,		1.0, 0.0,
	-0.5f, 0.5f, -0.5f,		0.0, 1.0, 0.0,		0.0, 1.0,

	-0.5f, 0.5f, -0.5f,		0.0, 1.0, 0.0,		0.0, 1.0,
	-0.5f, -0.5f, 0.5f,		0.0, 1.0, 0.0,		1.0, 0.0,
	-0.5f, 0.5f, 0.5f,		0.0, 1.0, 0.0,		1.0, 1.0,
	// 위쪽 면
	-0.5f, 0.5f, 0.5f,		0.0, 1.0, 0.0,		0.0, 0.0,
	0.5f, 0.5f, 0.5f,		0.0, 1.0, 0.0,		1.0, 0.0,
	-0.5f, 0.5f, -0.5f,		0.0, 1.0, 0.0,		0.0, 1.0,

	-0.5f, 0.5f, -0.5f,		0.0, 1.0, 0.0,		0.0, 1.0,
	0.5f, 0.5f, 0.5f,		0.0, 1.0, 0.0,		1.0, 0.0,
	0.5f, 0.5f, -0.5f,		0.0, 1.0, 0.0,		1.0, 1.0,
	// 아랫 면
	-0.5f, -0.5f, -0.5f,	0.0, 1.0, 0.0,		0.0, 0.0,
	0.5f, -0.5f, -0.5f,		0.0, 1.0, 0.0,		1.0, 0.0,
	-0.5f, -0.5f, 0.5f,		0.0, 1.0, 0.0,		0.0, 1.0,

	-0.5f, -0.5f, 0.5f,		0.0, 1.0, 0.0,		0.0, 1.0,
	0.5f, -0.5f, -0.5f,		0.0, 1.0, 0.0,		1.0, 0.0,
	0.5f, -0.5f, 0.5f,		0.0, 1.0, 0.0,		1.0, 1.0
};

GLfloat Pyramid[] = {
	// 앞면
	-0.5f, -0.5f, 0.5f,		0.0, 1.0, 0.0,		0.0, 0.0,
	0.5f, -0.5f, 0.5f,		0.0, 1.0, 0.0,		1.0, 0.0,
	0.0f, 0.5f, 0.0f,		0.0, 1.0, 0.0,		0.5, 1.0,

	// 오른쪽면
	0.5f, -0.5f, 0.5f,		0.0, 1.0, 0.0,		0.0, 0.0,
	0.5f, -0.5f, -0.5f,		0.0, 1.0, 0.0,		1.0, 0.0,
	0.0f, 0.5f, 0.0f,		0.0, 1.0, 0.0,		0.5, 1.0,

	// 뒷면
	0.5f, -0.5f, -0.5f,		0.0, 1.0, 0.0,		0.0, 0.0,
	-0.5f, -0.5f, -0.5f,	0.0, 1.0, 0.0,		1.0, 0.0,
	0.0f, 0.5f, 0.0f,		0.0, 1.0, 0.0,		0.5, 1.0,

	// 왼쪽면
	-0.5f, -0.5f, -0.5f,	0.0, 1.0, 0.0,		0.0, 0.0,
	-0.5f, -0.5f, 0.5f,		0.0, 1.0, 0.0,		1.0, 0.0,
	0.0f, 0.5f, 0.0f,		0.0, 1.0, 0.0,		0.5, 1.0,

	// 아랫면
	-0.5f, -0.5f, -0.5f,	0.0, 1.0, 0.0,		0.0, 0.0,
	0.5f, -0.5f, -0.5f,		0.0, 1.0, 0.0,		1.0, 0.0,
	-0.5f, -0.5f, 0.5f,		0.0, 1.0, 0.0,		0.0, 1.0,

	-0.5f, -0.5f, 0.5f,		0.0, 1.0, 0.0,		0.0, 1.0,
	0.5f, -0.5f, -0.5f,		0.0, 1.0, 0.0,		1.0, 0.0,
	0.5f, -0.5f, 0.5f,		0.0, 1.0, 0.0,		1.0, 1.0

};

GLfloat Lines[] = {
	0.0f, 1.5f, 0.0f,	0.0f, 0.0f, 0.0f,	0.0, 0.0,
	0.0f, -1.5f, 0.0f,	0.0f, 0.0f, 0.0f,	0.0, 0.0,
	-1.5f, 0.0f, 0.0f,	0.0f, 0.0f, 0.0f,	0.0, 0.0,
	1.5f, 0.0f, 0.0f,	0.0f, 0.0f, 0.0f,	0.0, 0.0,
	0.0f, 0.0f, -1.5f,	0.0f, 0.0f, 0.0f,	0.0, 0.0,
	0.0f, 0.0f, 1.5f,	0.0f, 0.0f, 0.0f,	0.0, 0.0,
};

GLfloat BackGround[] = {
	-1.0, -1.0, -1.0,	0.0, 0.0, 0.0,		0.0, 0.0,
	1.0, -1.0, -1.0,	0.0, 0.0, 0.0,		1.0, 0.0,
	-1.0, 1.0, -1.0,	0.0, 0.0, 0.0,		0.0, 1.0,

	-1.0, 1.0, -1.0,	0.0, 0.0, 0.0,		0.0, 1.0,
	1.0, -1.0, -1.0,	0.0, 0.0, 0.0,		1.0, 0.0,
	1.0, 1.0, -1.0,		0.0, 0.0, 0.0,		1.0, 1.0
};

int width;
int height;

GLuint vertexShader;
GLuint fragmentShader;

unsigned int textures[7];

const char* texturename[7] = { "objs/A.bmp", "objs/B.bmp", "objs/C.bmp", "objs/D.bmp", "objs/E.bmp", "objs/F.bmp", "objs/L.bmp" };
GLuint shaderID;

int type = 1;

void InitTexture() {
	int widthImage, heightImage, numberOfChannel;
	for (int i = 0; i < 7; ++i) {
		glGenTextures(1, &textures[i]);
		glBindTexture(GL_TEXTURE_2D, textures[i]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		stbi_set_flip_vertically_on_load(true);
		unsigned char* data = stbi_load(texturename[i], &widthImage, &heightImage, &numberOfChannel, 0);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, widthImage, heightImage, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		stbi_image_free(data);
	}

	glUseProgram(shaderID);
	int tLocation = glGetUniformLocation(shaderID, "texture1");
	glUniform1i(tLocation, 0);
}

void make_vertexShaders() {
	// 버텍스 세이더 읽어 저장하고 컴파일 하기
	GLchar* vertexSource;
	vertexSource = filetobuf("vertex10-23.glsl");

	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);

	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
	if (!result) {
		glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
		cerr << "ERROR:vertex shader 컴파일 실패\n" << errorLog << endl;
	}
}

void make_fragmentShaders() {
	GLchar* fragmentsource;
	fragmentsource = filetobuf("fragment10-23.glsl");
	// 프래그먼트 세이더 읽어 저장하고 컴파일하기
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentsource, NULL);
	glCompileShader(fragmentShader);

	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (!result) {
		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
		cerr << "ERROR:fragment shader 컴파일 실패\n" << errorLog << endl;
	}
}

glm::mat4 transformMatrix(1.0f);

void InitShader() {
	make_vertexShaders();
	make_fragmentShaders();
	shaderID = glCreateProgram();

	glAttachShader(shaderID, vertexShader);
	glAttachShader(shaderID, fragmentShader);
	glLinkProgram(shaderID);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	glUseProgram(shaderID);
}

unsigned int transformLocation;

glm::mat4 projection = glm::mat4(1.0f);
unsigned int projectionLocation;
glm::mat4 view = glm::mat4(1.0f);
unsigned int viewLocation;

glm::mat4 rotateM(1.0f);

glm::vec3 cameraPos = glm::vec3(-2.0f, 2.0f, 3.0f);
glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

unsigned int isTransLocation;

void main(int argc, char** argv) {
	width = 600;
	height = 600;

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(width, height);
	glutCreateWindow("practice25");

	glewExperimental = GL_TRUE;
	glewInit();

	InitShader();
	InitTexture();
	InitBuffer();

	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glutKeyboardFunc(Keyboard);
	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutTimerFunc(30, Timer, 1);

	isTransLocation = glGetUniformLocation(shaderID, "Trans");

	transformLocation = glGetUniformLocation(shaderID, "g_modelTransform");

	projection = glm::perspective(glm::radians(45.0f), (float)GLUT_SCREEN_WIDTH / (float)GLUT_SCREEN_HEIGHT, 0.1f, 50.0f);
	projectionLocation = glGetUniformLocation(shaderID, "g_projection");

	view = glm::lookAt(cameraPos, cameraDirection, cameraUp);
	viewLocation = glGetUniformLocation(shaderID, "g_view");

	glutMainLoop();
}

GLvoid drawScene() {
	static glm::mat4 tmp(1.0f);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shaderID);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textures[6]);
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	glUniform1i(isTransLocation, 0);
	glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
	glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
	glBindVertexArray(VAO[3]);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(rotateM));
	glUniform1i(isTransLocation, 1);
	glBindVertexArray(VAO[0]);
	if (type == 1) {
		for (int i = 0; i < 6; ++i) {
			glBindTexture(GL_TEXTURE_2D, textures[i]);
			glDrawArrays(GL_TRIANGLES, i * 6, 6);
		}
	}
	else if (type == 2) {
		glBindVertexArray(VAO[1]);
		for (int i = 0; i < 4; ++i) {
			glBindTexture(GL_TEXTURE_2D, textures[i]);
			glDrawArrays(GL_TRIANGLES, i * 3, 3);
		}
		glBindTexture(GL_TEXTURE_2D, textures[4]);
		glDrawArrays(GL_TRIANGLES, 12, 6);
	}
	glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
	glBindVertexArray(VAO[2]);
	glDrawArrays(GL_LINES, 0, 6);


	glutSwapBuffers();
}

GLvoid Reshape(int w, int h) {							//---콜백함수 : 다시 그리기 콜백 함수
	glViewport(0, 0, w, h);
}

GLvoid InitBuffer() {
	glGenVertexArrays(4, VAO);
	glGenBuffers(4, VBO);

	// 도형 VAO
	glBindVertexArray(VAO[0]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Cube), Cube, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(VAO[1]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Pyramid), Pyramid, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(VAO[2]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Lines), Lines, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(VAO[3]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[3]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(BackGround), BackGround, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
}


GLvoid Keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'c':
		type = 1;
		break;
	case 'p':
		type = 2;
		break;
	case 'x':
		rotateM = glm::rotate(rotateM, glm::radians(5.0f), glm::vec3(1.0, 0.0, 0.0));
		break;
	case 'X':
		rotateM = glm::rotate(rotateM, glm::radians(-5.0f), glm::vec3(1.0, 0.0, 0.0));
		break;
	case 'y':
		rotateM = glm::rotate(rotateM, glm::radians(5.0f), glm::vec3(0.0, 1.0, 0.0));
		break;
	case 'Y':
		rotateM = glm::rotate(rotateM, glm::radians(-5.0f), glm::vec3(0.0, 1.0, 0.0));
		break;
	case 's':
		rotateM = glm::mat4(1.0f);
		break;
	}
	glutPostRedisplay();
}

GLvoid Timer(int value) {
	glutPostRedisplay();
	glutTimerFunc(30, Timer, 1);
}
