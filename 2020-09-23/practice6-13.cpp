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
#define CLR(a) {clr[a][0], clr[a][1], clr[a][2]}

using namespace std;

GLvoid drawScene();
GLvoid Reshape(int w, int h);
GLvoid Timer(int value);
GLvoid InitBuffer();
GLvoid InitShader();
GLvoid UpdateBuffer();
GLvoid Timer(int value);
GLvoid Keyboard(unsigned char key, int x, int y);

GLuint VAO[3], VBO[3];

random_device rd;
default_random_engine dre(rd());
uniform_real_distribution<> urd1(-0.7f, 0.7f);
uniform_real_distribution<> urd2(0.005f, 0.02f);
uniform_real_distribution<> urdColor(0.0f, 1.0f);
GLfloat clr[9][3] = {
	rndclr, rndclr,rndclr,rndclr,rndclr,rndclr,rndclr,rndclr,rndclr
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


GLfloat Lines[4][2][3] = {
	{{0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},
	{{0.0f, -1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},
	{{-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},
	{{1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},
};

int width;
int height;

int type = 1; // 1 : 육면체	2 : 피라미드
int rotateX = 0;	// 0 : 정지	1 : 양의 방향 회전		2 : 음의 방향 회전
int rotateY = 0;	// 0 : 정지	1 : 양의 방향 회전		2 : 음의 방향 회전
bool isWire = false;
bool isHide = true;

GLuint vertexShader;
GLuint fragmentShader;

void make_vertexShaders() {
	// 버텍스 세이더 읽어 저장하고 컴파일 하기
	GLchar* vertexSource;
	vertexSource = filetobuf("vertex6-12.glsl");

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
glm::mat4 ttx(1.0f);
glm::mat4 tty(1.0f);
glm::mat4 ttz(1.0f);
glm::mat4 rx(1.0f);
glm::mat4 ry(1.0f);

unsigned int transformLocation;

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

void main(int argc, char** argv) {
	width = 600;
	height = 600;

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(width, height);
	glutCreateWindow("practice12");

	glewExperimental = GL_TRUE;
	glewInit();

	InitShader();
	InitBuffer();

	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glutKeyboardFunc(Keyboard);
	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutTimerFunc(30, Timer, 1);

	transformLocation = glGetUniformLocation(shaderID, "transform");

	tx = glm::translate(tx, glm::vec3(-0.5f, 0.0f, 0.0f));
	ty = glm::translate(ty, glm::vec3(0.0f, -0.5f, 0.0f));
	tz = glm::translate(tz, glm::vec3(0.0f, 0.0f, -0.5f));
	rx = glm::rotate(rx, glm::radians(30.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	ry = glm::rotate(ry, glm::radians(-30.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	glutMainLoop();
}

GLvoid drawScene() {
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shaderID);

	glBegin(GL_LINE_STRIP);
	static float x, y, z;

	x = -1.0f;
	for (float angle = 0.0f; angle <= (2.0f * 3.1415f) * 2.0f; angle += 0.1f)
	{
		y = 0.5f * sin(angle);
		x = angle / (2.0f * 3.1415f) - 1.0f;
		// Specify the point and move the Z value up a little 
		glVertex3f(x, y, 0.0f);
	}
	glEnd();

	glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
	glBindVertexArray(VAO[1]);
	glDrawArrays(GL_LINES, 0, 4);

	glutSwapBuffers();
}

GLvoid Reshape(int w, int h) {							//---콜백함수 : 다시 그리기 콜백 함수
	glViewport(0, 0, w, h);
}

GLvoid InitBuffer() {
	glGenVertexArrays(3, VAO);
	glGenBuffers(3, VBO);

	// 좌표계 VAO
	glBindVertexArray(VAO[1]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);

	glBufferData(GL_ARRAY_BUFFER, sizeof(Lines), Lines, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
}


GLvoid Keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'c':
		type = 1;
		break;
	case 'p':
		type = 2;
		break;
	case 'h':
		if (isHide) {
			isHide = false;
			glDisable(GL_DEPTH_TEST);
		}
		else {
			isHide = true;
			glEnable(GL_DEPTH_TEST);
		}
		break;
	case 'x':
		rotateX = 1;
		break;
	case 'X':
		rotateX = 2;
		break;
	case 'y':
		rotateY = 1;
		break;
	case 'Y':
		rotateY = 2;
		break;
	case 'w':
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		break;
	case 'W':
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;
	}
	glutPostRedisplay();
}

GLvoid Timer(int value) {
	if (rotateX == 1) {
		rx = glm::rotate(rx, glm::radians(1.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	}
	else if (rotateX == 2) {
		rx = glm::rotate(rx, glm::radians(-1.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	}
	if (rotateY == 1) {
		ry = glm::rotate(ry, glm::radians(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	}
	else if (rotateY == 2) {
		ry = glm::rotate(ry, glm::radians(-1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	}
	transformMatrix = ttx * tty * ttz * rx * ry * tx * ty * tz;
	glutPostRedisplay();
	glutTimerFunc(30, Timer, 1);
}