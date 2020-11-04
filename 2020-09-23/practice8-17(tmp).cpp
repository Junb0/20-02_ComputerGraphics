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

#define rndclr {urdColor(dre), urdColor(dre), urdColor(dre)}
#define CLR(a) {clr[a - 1][0], clr[a - 1][1], clr[a - 1][2]}

using namespace std;

GLvoid drawScene();
GLvoid Reshape(int w, int h);
GLvoid Timer(int value);
GLvoid InitBuffer();
GLvoid InitShader();
GLvoid UpdateBuffer();
GLvoid Timer(int value);
GLvoid Keyboard(unsigned char key, int x, int y);

GLuint VAO, VBO, EBO;

random_device rd;
default_random_engine dre(rd());
uniform_real_distribution<> urd1(-0.7f, 0.7f);
uniform_real_distribution<> urd2(0.005f, 0.02f);
uniform_real_distribution<> urdColor(0.0f, 1.0f);
GLfloat clr[6][3] = {
	rndclr,rndclr,rndclr,rndclr,rndclr,rndclr
};

class Face {
public:
	unsigned int Vindices[3];
	unsigned int VNindices[3];
	unsigned int Tindices[3];
};

class Object {
public:
	vector<glm::vec3> vertex;
	vector<Face> face;
	GLfloat color[3];
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

Object ReadObj(const char* file) {
	// 0. 파일 오픈
	FILE* objFile;
	objFile = fopen(file, "r");
	if (!objFile) {
		cout << "cannot open object file" << endl;
		return Object{};
	}

	// 1. 전체 버텍스 개수 및 삼각형 개수 세기

	// 2. 메모리 할당
	vector<glm::vec3> vertex;
	vector<Face> face;
	
	// 3. 할당된 메모리에 각 버텍스, 페이스 정보 입력
	char bind[128];
	glm::vec3 Tmp;
	Face fTmp;
	while (!feof(objFile)) {
		fscanf(objFile, "%s", bind);
		if (bind[0] == 'v' && bind[1] == '\0') {
			fscanf(objFile, "%f %f %f", &Tmp.x, &Tmp.y, &Tmp.z);
			vertex.push_back(Tmp);
		}
		else if (bind[0] == 'f' && bind[1] == '\0') {
			fscanf(objFile, "%u/%u/%u %u/%u/%u %u/%u/%u", &fTmp.Vindices[0], &fTmp.Tindices[0], &fTmp.VNindices[0], &fTmp.Vindices[1], &fTmp.Tindices[1], &fTmp.VNindices[1], &fTmp.Vindices[2], &fTmp.Tindices[2], &fTmp.VNindices[2]);
			face.push_back(fTmp);
		}
	}
	Object obj{ vertex, face };
	fclose(objFile);
	return obj;
}

int width;
int height;

bool isWire = false;
bool isHide = true;
int isCubeLidRotate = 0;
int isCubeFrontOpen = 0;
int isPyramidOpen = 0;

GLuint vertexShader;
GLuint fragmentShader;

void make_vertexShaders() {
	// 버텍스 세이더 읽어 저장하고 컴파일 하기
	GLchar* vertexSource;
	vertexSource = filetobuf("vertex8-17.glsl");

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
	fragmentsource = filetobuf("fragment6-12.glsl");
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

GLuint shaderID;
glm::mat4 transformMatrix(1.0f);
glm::mat4 tx(1.0f);
glm::mat4 ty(1.0f);
glm::mat4 tz(1.0f);
glm::mat4 rx(1.0f);
glm::mat4 ry(1.0f);


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

unsigned int colorLocation;

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

Object box1;

void main(int argc, char** argv) {
	width = 600;
	height = 600;

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(width, height);
	glutCreateWindow("practice17");

	glewExperimental = GL_TRUE;
	glewInit();

	colorLocation = glGetUniformLocation(shaderID, "color");

	box1 = ReadObj("objs/box1.obj");
	box1.color[0] = 1.0f;
	box1.color[1] = 1.0f;
	box1.color[2] = 0.0f;

	InitShader();
	InitBuffer();

	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glutKeyboardFunc(Keyboard);
	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutTimerFunc(30, Timer, 1);

	tx = glm::translate(tx, glm::vec3(-0.5f, 0.0f, 0.0f));
	ty = glm::translate(ty, glm::vec3(0.0f, -0.5f, 0.0f));
	tz = glm::translate(tz, glm::vec3(0.0f, 0.0f, -0.5f));
	rx = glm::rotate(rx, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	ry = glm::rotate(ry, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	transformLocation = glGetUniformLocation(shaderID, "modelTransform");

	projection = glm::perspective(glm::radians(45.0f), (float)GLUT_SCREEN_WIDTH / (float)GLUT_SCREEN_HEIGHT, 0.1f, 50.0f);
	projectionLocation = glGetUniformLocation(shaderID, "projectionTransform");

	view = glm::lookAt(cameraPos, cameraDirection, cameraUp);
	viewLocation = glGetUniformLocation(shaderID, "viewTransform");

	

	glutMainLoop();
}

GLvoid drawScene() {
	static glm::mat4 tmp(1.0f);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shaderID);

	glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));

	glUniform3fv(colorLocation, 1, &box1.color[0]);

	glDrawElements(GL_TRIANGLES, box1.face.size() * 3, GL_UNSIGNED_INT, nullptr);

	glutSwapBuffers();
}

GLvoid Reshape(int w, int h) {							//---콜백함수 : 다시 그리기 콜백 함수
	glViewport(0, 0, w, h);
}

GLvoid InitBuffer() {
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * box1.vertex.size(), box1.vertex.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Face) * box1.face.size(), box1.face.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
	glEnableVertexAttribArray(0);
}


GLvoid Keyboard(unsigned char key, int x, int y) {
	switch (key) {
	}
	glutPostRedisplay();
}

GLvoid Timer(int value) {
	glutPostRedisplay();
	glutTimerFunc(30, Timer, 1);
}
