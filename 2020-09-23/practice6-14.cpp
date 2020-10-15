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

GLfloat Cube[12][3][2][3] = {
	{{v1, CLR(1)}, {v7, CLR(7)}, {v5, CLR(5)}},
	{{v1, CLR(1)}, {v3, CLR(3)}, {v7, CLR(7)}},
	{{v1, CLR(1)}, {v4, CLR(4)}, {v3, CLR(3)}},
	{{v1, CLR(1)}, {v2, CLR(2)}, {v4, CLR(4)}},
	{{v3, CLR(3)}, {v8, CLR(8)}, {v7, CLR(7)}},
	{{v3, CLR(3)}, {v4, CLR(4)}, {v8, CLR(8)}},
	{{v5, CLR(5)}, {v7, CLR(7)}, {v8, CLR(8)}},
	{{v5, CLR(5)}, {v8, CLR(8)}, {v6, CLR(6)}},
	{{v1, CLR(1)}, {v5, CLR(5)}, {v6, CLR(6)}},
	{{v1, CLR(1)}, {v6, CLR(6)}, {v2, CLR(2)}},
	{{v2, CLR(2)}, {v6, CLR(6)}, {v8, CLR(8)}},
	{{v2, CLR(2)}, {v8, CLR(8)}, {v4, CLR(4)}},
};

GLfloat Pyramid[6][3][2][3] = {
	{{v9, CLR(9)}, {v1, CLR(1)}, {v5, CLR(5)}},
	{{v9, CLR(9)}, {v5, CLR(5)}, {v6, CLR(6)}},
	{{v9, CLR(9)}, {v6, CLR(6)}, {v2, CLR(2)}},
	{{v9, CLR(9)}, {v2, CLR(2)}, {v1, CLR(1)}},
	{{v1, CLR(1)}, {v5, CLR(5)}, {v2, CLR(2)}},
	{{v2, CLR(2)}, {v5, CLR(5)}, {v6, CLR(6)}},
};

GLfloat Lines[6][2][3] = {
	{{0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},
	{{0.0f, -1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},
	{{-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},
	{{1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},
	{{0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},
	{{0.0f, 0.0f, -1.0f}, {0.0f, 0.0f, 0.0f}},
};

GLUquadricObj* qobj;

int width;
int height;

int type = 1; // 1 : 육면체	2 : 피라미드
int rotateX = 0;	// 0 : 정지	1 : 양의 방향 회전		2 : 음의 방향 회전
int rotateY = 0;	// 0 : 정지	1 : 양의 방향 회전		2 : 음의 방향 회전
int rotateB = 0;
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
glm::mat4 transformMatrix1(1.0f);
glm::mat4 tx1(1.0f);
glm::mat4 ty1(1.0f);
glm::mat4 tz1(1.0f);
glm::mat4 rx1(1.0f);
glm::mat4 ry1(1.0f);
glm::mat4 s1(1.0f);


glm::mat4 transformMatrix2(1.0f);
glm::mat4 tx2(1.0f);
glm::mat4 ty2(1.0f);
glm::mat4 tz2(1.0f);
glm::mat4 rx2(1.0f);
glm::mat4 ry2(1.0f);
glm::mat4 s2(1.0f);

glm::mat4 transformMatrix3(1.0f);
glm::mat4 tx3(1.0f);
glm::mat4 ty3(1.0f);
glm::mat4 tz3(1.0f);
glm::mat4 rx3(1.0f);
glm::mat4 ry3(1.0f);

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
	glutCreateWindow("practice14");

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

	tx1 = glm::translate(tx1, glm::vec3(0.25f, 0.0f, 0.0f));
	ty1 = glm::translate(ty1, glm::vec3(0.0f, -0.25f, 0.0f));
	tz1 = glm::translate(tz1, glm::vec3(0.0f, 0.0f, -0.25f));
	s1 = glm::scale(s1, glm::vec3(0.5f, 0.5f, 0.5f));

	tx2 = glm::translate(tx2, glm::vec3(-0.5f, 0.0f, 0.0f));
	ty2 = glm::translate(ty2, glm::vec3(0.0f, 0.0f, 0.0f));
	tz2 = glm::translate(tz2, glm::vec3(0.0f, 0.0f, 0.0f));
	s2 = glm::scale(s2, glm::vec3(0.5f, 0.5f, 0.5f));

	rx3 = glm::rotate(rx3, glm::radians(-30.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	ry3 = glm::rotate(ry3, glm::radians(10.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	qobj = gluNewQuadric();
	gluQuadricDrawStyle(qobj, GLU_FILL);
	glutMainLoop();
}

GLvoid drawScene() {
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shaderID);

	transformMatrix1 = rx3 * ry3 * rx1 * ry1 * tx1 * ty1 * tz1  * s1;
	transformMatrix2 = rx3 * ry3 * tx2 * ty2 * tz2 * s2 * rx2 * ry2;
	transformMatrix3 = rx3 * ry3;

	glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(transformMatrix1));
	if (type == 1) {
		glBindVertexArray(VAO[0]);	// 삼각형의 속성이 있는 VAO[0] 사용
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
	else if (type == 2) {
		glBindVertexArray(VAO[2]);	// 삼각형의 속성이 있는 VAO[0] 사용
		glDrawArrays(GL_TRIANGLES, 0, 18);
	}

	glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(transformMatrix2));
	if(type == 1)
		gluSphere(qobj, 0.5, 20, 20);
	else if(type == 2)
		gluDisk(qobj, 0.0, 0.5, 20, 3);

	glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(transformMatrix3));
	glBindVertexArray(VAO[1]);
	glDrawArrays(GL_LINES, 0, 6);

	glutSwapBuffers();
}

GLvoid Reshape(int w, int h) {							//---콜백함수 : 다시 그리기 콜백 함수
	glViewport(0, 0, w, h);
}

GLvoid InitBuffer() {
	glGenVertexArrays(3, VAO);
	glGenBuffers(3, VBO);

	// 도형 VAO
	glBindVertexArray(VAO[0]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);

	glBufferData(GL_ARRAY_BUFFER, sizeof(Cube), Cube, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// 좌표계 VAO
	glBindVertexArray(VAO[1]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);

	glBufferData(GL_ARRAY_BUFFER, sizeof(Lines), Lines, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);


	glBindVertexArray(VAO[2]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);

	glBufferData(GL_ARRAY_BUFFER, sizeof(Pyramid), Pyramid, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
}


GLvoid Keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'C':
		if (type == 1)
			type = 2;
		else if (type == 2)
			type = 1;
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
	case 'b':
		rotateB = 1;
		break;
	case 'B':
		rotateB = 2;
		break;
	case 'w':
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		break;
	case 'W':
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;
	case 'S':
		rotateX = 0;
		rotateY = 0;
		rotateB = 0;
	}
	glutPostRedisplay();
}

GLvoid Timer(int value) {
	if (rotateX == 1) {
		rx1 = glm::rotate(rx1, glm::radians(1.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	}
	else if (rotateX == 2) {
		rx1 = glm::rotate(rx1, glm::radians(-1.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	}
	if (rotateY == 1) {
		ry2 = glm::rotate(ry2, glm::radians(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	}
	else if (rotateY == 2) {
		ry2 = glm::rotate(ry2, glm::radians(-1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	}
	if (rotateB == 1) {
		ry3 = glm::rotate(ry3, glm::radians(0.8f), glm::vec3(0.0f, 1.0f, 0.0f));
	}
	else if (rotateB == 2) {
		ry3 = glm::rotate(ry3, glm::radians(-0.8f), glm::vec3(0.0f, 1.0f, 0.0f));
	}
	glutPostRedisplay();
	glutTimerFunc(30, Timer, 1);
}
