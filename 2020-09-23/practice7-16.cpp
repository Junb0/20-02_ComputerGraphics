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

GLuint VAO[3], VBO[3];

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

GLfloat Cube[12][3][2][3] = {	// 카메라 기준
	{{v1, CLR(1)}, {v7, CLR(1)}, {v5, CLR(1)}},	//뒷면
	{{v1, CLR(1)}, {v3, CLR(1)}, {v7, CLR(1)}},

	{{v1, CLR(2)}, {v4, CLR(2)}, {v3, CLR(2)}},
	{{v1, CLR(2)}, {v2, CLR(2)}, {v4, CLR(2)}},

	{{v3, CLR(3)}, {v8, CLR(3)}, {v7, CLR(3)}},	// 윗면
	{{v3, CLR(3)}, {v4, CLR(3)}, {v8, CLR(3)}},

	{{v5, CLR(4)}, {v7, CLR(4)}, {v8, CLR(4)}},
	{{v5, CLR(4)}, {v8, CLR(4)}, {v6, CLR(4)}},

	{{v1, CLR(5)}, {v5, CLR(5)}, {v6, CLR(5)}},
	{{v1, CLR(5)}, {v6, CLR(5)}, {v2, CLR(5)}},

	{{v2, CLR(6)}, {v6, CLR(6)}, {v8, CLR(6)}},
	{{v2, CLR(6)}, {v8, CLR(6)}, {v4, CLR(6)}},
};

GLfloat Pyramid[6][3][2][3] = {
	{{v9, CLR(1)}, {v1, CLR(1)}, {v5, CLR(1)}},
	{{v9, CLR(2)}, {v5, CLR(2)}, {v6, CLR(2)}},
	{{v9, CLR(3)}, {v6, CLR(3)}, {v2, CLR(3)}},
	{{v9, CLR(4)}, {v2, CLR(4)}, {v1, CLR(4)}},
	{{v1, CLR(5)}, {v5, CLR(5)}, {v2, CLR(5)}},
	{{v2, CLR(5)}, {v5, CLR(5)}, {v6, CLR(5)}},
};

GLfloat Lines[6][2][3] = {
	{{0.0f, 1.5f, 0.0f}, {0.0f, 0.0f, 0.0f}},
	{{0.0f, -1.5f, 0.0f}, {0.0f, 0.0f, 0.0f}},
	{{-1.5f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},
	{{1.5f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},
	{{0.0f, 0.0f, -1.5f}, {0.0f, 0.0f, 0.0f}},
	{{0.0f, 0.0f, 1.5f}, {0.0f, 0.0f, 0.0f}},
};

int width;
int height;

int type = 1; // 1 : 육면체	2 : 피라미드
int rotateX = 0;	// 0 : 정지	1 : 양의 방향 회전		2 : 음의 방향 회전
int rotateY = 0;	// 0 : 정지	1 : 양의 방향 회전		2 : 음의 방향 회전
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
	vertexSource = filetobuf("vertex7-16.glsl");

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

glm::mat4 lidRotateMatrix(1.0f);
glm::mat4 frontRotateMatrix(1.0f);

glm::mat4 backpyramidRotateMatrix(1.0f);
glm::mat4 frontpyramidRotateMatrix(1.0f);
glm::mat4 leftpyramidRotateMatrix(1.0f);
glm::mat4 rightpyramidRotateMatrix(1.0f);

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

glm::vec3 cameraPos = glm::vec3(-2.0f, 2.0f, 3.0f);
glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

void main(int argc, char** argv) {
	width = 600;
	height = 600;

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(width, height);
	glutCreateWindow("practice16");

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

	glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(transformMatrix));
	if (type == 1) {
		glBindVertexArray(VAO[0]);	// 삼각형의 속성이 있는 VAO[0] 사용
		glDrawArrays(GL_TRIANGLES, 0, 12);
		// 윗면 돌리기
		tmp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, -0.5f));
		tmp = transformMatrix * glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, 0.5f)) * lidRotateMatrix * tmp;
		glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(tmp));
		glDrawArrays(GL_TRIANGLES, 12, 6);
		// 윗면 돌리기 끝
		glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(transformMatrix));
		glDrawArrays(GL_TRIANGLES, 18, 12);

		// 앞면 열기
		tmp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -1.0f));
		tmp = transformMatrix * glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * frontRotateMatrix * tmp;
		glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(tmp));
		glDrawArrays(GL_TRIANGLES, 30, 6);	// 앞면
	}
	else if (type == 2) {
		glBindVertexArray(VAO[2]);	// 삼각형의 속성이 있는 VAO[0] 사용
		tmp = glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f, 0.0f, 0.0f));
		tmp = transformMatrix * glm::translate(glm::mat4(1.0f), glm::vec3(0.5f, 0.0f, 0.0f)) * backpyramidRotateMatrix * tmp;
		glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(tmp));
		glDrawArrays(GL_TRIANGLES, 0, 3);		// 뒷면

		tmp = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 0.0f, 0.0f));
		tmp = transformMatrix * glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 0.0f, 0.0f)) * rightpyramidRotateMatrix * tmp;
		glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(tmp));
		glDrawArrays(GL_TRIANGLES, 3, 3);		// 오른쪽

		tmp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -1.0f));
		tmp = transformMatrix * glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * frontpyramidRotateMatrix * tmp;
		glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(tmp));
		glDrawArrays(GL_TRIANGLES, 6, 3);		// 앞면

		tmp = transformMatrix * leftpyramidRotateMatrix;
		glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(tmp));
		glDrawArrays(GL_TRIANGLES, 9, 3);		// 왼쪽

		glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(transformMatrix));
		glDrawArrays(GL_TRIANGLES, 12, 6);
	}

	glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
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
	case 'a':
		type = 1;
		break;
	case 's':
		type = 2;
		break;
	case 't':
		if (type == 1)
			isCubeLidRotate = 1;
		break;
	case 'T':
		if (type == 1)
			isCubeLidRotate = 0;
		break;
	case 'f':
		if (type == 1)
			isCubeFrontOpen = 1;
		break;
	case 'F':
		if (type == 1)
			isCubeFrontOpen = 2;
		break;
	case 'y':
		if (rotateY == 0)
			rotateY = 1;
		else
			rotateY = 0;
		break;
	case 'w':
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		break;
	case 'W':
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;
	case 'o':
		if (type == 2)
			isPyramidOpen = 1;
		break;
	case 'O':
		if (type == 2)
			isPyramidOpen = 2;
		break;
	case 'p':
		view = glm::mat4(1.0f);
		projection = glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, -2.0f, 2.0f);
		break;
	case 'P':
		view = glm::lookAt(cameraPos, cameraDirection, cameraUp);
		projection = glm::perspective(glm::radians(45.0f), (float)GLUT_SCREEN_WIDTH / (float)GLUT_SCREEN_HEIGHT, 0.1f, 50.0f);
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
	static float FrontRad = 0.0f;
	if (isCubeFrontOpen == 1) {
		if (FrontRad < 90.0f) {
			frontRotateMatrix = glm::rotate(frontRotateMatrix, glm::radians(1.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			FrontRad += 1.0f;
		}
		else
			isCubeFrontOpen = 0;
	}
	if (isCubeFrontOpen == 2) {
		if (FrontRad > 0.0f) {
			frontRotateMatrix = glm::rotate(frontRotateMatrix, glm::radians(-1.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			FrontRad -= 1.0f;
		}
		else
			isCubeFrontOpen = 0;
	}
	if (isCubeLidRotate == 1)
		lidRotateMatrix = glm::rotate(lidRotateMatrix, glm::radians(1.0f), glm::vec3(1.0f, 0.0f, 0.0f));

	static float PyramidRad = 0.0f;

	if (isPyramidOpen == 1) {
		if (PyramidRad < 90.0f) {
			backpyramidRotateMatrix = glm::rotate(backpyramidRotateMatrix, glm::radians(-1.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			rightpyramidRotateMatrix = glm::rotate(rightpyramidRotateMatrix, glm::radians(-1.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			frontpyramidRotateMatrix = glm::rotate(frontpyramidRotateMatrix, glm::radians(1.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			leftpyramidRotateMatrix = glm::rotate(leftpyramidRotateMatrix, glm::radians(1.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			PyramidRad += 1.0f;
		}
		else
			isPyramidOpen == 0;
	}
	if (isPyramidOpen == 2) {
		if (PyramidRad > 0.0f) {
			backpyramidRotateMatrix = glm::rotate(backpyramidRotateMatrix, glm::radians(1.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			rightpyramidRotateMatrix = glm::rotate(rightpyramidRotateMatrix, glm::radians(1.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			frontpyramidRotateMatrix = glm::rotate(frontpyramidRotateMatrix, glm::radians(-1.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			leftpyramidRotateMatrix = glm::rotate(leftpyramidRotateMatrix, glm::radians(-1.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			PyramidRad -= 1.0f;
		}
		else
			isPyramidOpen == 0;
	}

	transformMatrix =  rx * ry * tx * ty * tz;
	glutPostRedisplay();
	glutTimerFunc(30, Timer, 1);
}
