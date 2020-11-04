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

GLfloat Cube1[12][3][2][3] = {	// 카메라 기준
	{{v1, CLR(1)}, {v7, CLR(1)}, {v5, CLR(1)}},	//뒷면
	{{v1, CLR(1)}, {v3, CLR(1)}, {v7, CLR(1)}},
	{{v1, CLR(1)}, {v4, CLR(1)}, {v3, CLR(1)}},
	{{v1, CLR(1)}, {v2, CLR(1)}, {v4, CLR(1)}},	  							  
	{{v3, CLR(1)}, {v8, CLR(1)}, {v7, CLR(1)}},	// 윗면
	{{v3, CLR(1)}, {v4, CLR(1)}, {v8, CLR(1)}},
	{{v5, CLR(1)}, {v7, CLR(1)}, {v8, CLR(1)}},
	{{v5, CLR(1)}, {v8, CLR(1)}, {v6, CLR(1)}},
	{{v1, CLR(1)}, {v5, CLR(1)}, {v6, CLR(1)}},
	{{v1, CLR(1)}, {v6, CLR(1)}, {v2, CLR(1)}},
	{{v2, CLR(1)}, {v6, CLR(1)}, {v8, CLR(1)}},
	{{v2, CLR(1)}, {v8, CLR(1)}, {v4, CLR(1)}},
};

GLfloat Cube2[12][3][2][3] = {	// 카메라 기준
	{{v1, CLR(2)}, {v7, CLR(2)}, {v5, CLR(2)}},	//뒷면
	{{v1, CLR(2)}, {v3, CLR(2)}, {v7, CLR(2)}},
	{{v1, CLR(2)}, {v4, CLR(2)}, {v3, CLR(2)}},
	{{v1, CLR(2)}, {v2, CLR(2)}, {v4, CLR(2)}},
	{{v3, CLR(2)}, {v8, CLR(2)}, {v7, CLR(2)}},	// 윗면
	{{v3, CLR(2)}, {v4, CLR(2)}, {v8, CLR(2)}},
	{{v5, CLR(2)}, {v7, CLR(2)}, {v8, CLR(2)}},
	{{v5, CLR(2)}, {v8, CLR(2)}, {v6, CLR(2)}},
	{{v1, CLR(2)}, {v5, CLR(2)}, {v6, CLR(2)}},
	{{v1, CLR(2)}, {v6, CLR(2)}, {v2, CLR(2)}},
	{{v2, CLR(2)}, {v6, CLR(2)}, {v8, CLR(2)}},
	{{v2, CLR(2)}, {v8, CLR(2)}, {v4, CLR(2)}},
};

GLfloat Cube3[12][3][2][3] = {	// 카메라 기준
	{{v1, CLR(3)}, {v7, CLR(3)}, {v5, CLR(3)}},	//뒷면
	{{v1, CLR(3)}, {v3, CLR(3)}, {v7, CLR(3)}},
	{{v1, CLR(3)}, {v4, CLR(3)}, {v3, CLR(3)}},
	{{v1, CLR(3)}, {v2, CLR(3)}, {v4, CLR(3)}},
	{{v3, CLR(3)}, {v8, CLR(3)}, {v7, CLR(3)}},	// 윗면
	{{v3, CLR(3)}, {v4, CLR(3)}, {v8, CLR(3)}},
	{{v5, CLR(3)}, {v7, CLR(3)}, {v8, CLR(3)}},
	{{v5, CLR(3)}, {v8, CLR(3)}, {v6, CLR(3)}},
	{{v1, CLR(3)}, {v5, CLR(3)}, {v6, CLR(3)}},
	{{v1, CLR(3)}, {v6, CLR(3)}, {v2, CLR(3)}},
	{{v2, CLR(3)}, {v6, CLR(3)}, {v8, CLR(3)}},
	{{v2, CLR(3)}, {v8, CLR(3)}, {v4, CLR(3)}},
};

GLfloat Cube4[12][3][2][3] = {	// 카메라 기준
	{{v1, CLR(4)}, {v7, CLR(4)}, {v5, CLR(4)}},	//뒷면
	{{v1, CLR(4)}, {v3, CLR(4)}, {v7, CLR(4)}},
	{{v1, CLR(4)}, {v4, CLR(4)}, {v3, CLR(4)}},
	{{v1, CLR(4)}, {v2, CLR(4)}, {v4, CLR(4)}},
	{{v3, CLR(4)}, {v8, CLR(4)}, {v7, CLR(4)}},	// 윗면
	{{v3, CLR(4)}, {v4, CLR(4)}, {v8, CLR(4)}},
	{{v5, CLR(4)}, {v7, CLR(4)}, {v8, CLR(4)}},
	{{v5, CLR(4)}, {v8, CLR(4)}, {v6, CLR(4)}},
	{{v1, CLR(4)}, {v5, CLR(4)}, {v6, CLR(4)}},
	{{v1, CLR(4)}, {v6, CLR(4)}, {v2, CLR(4)}},
	{{v2, CLR(4)}, {v6, CLR(4)}, {v8, CLR(4)}},
	{{v2, CLR(4)}, {v8, CLR(4)}, {v4, CLR(4)}},
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

glm::mat4 box1Scale(1.0f);
glm::mat4 box2Scale(1.0f);
glm::mat4 box3Scale(1.0f);
glm::mat4 box4Scale(1.0f);
glm::mat4 box5Scale(1.0f);

glm::mat4 box1Translate(1.0f);
glm::mat4 box2Translate(1.0f);
glm::mat4 box3Translate(1.0f);
glm::mat4 box4Translate(1.0f);
glm::mat4 box5Translate(1.0f);

glm::mat4 machineTranslate(1.0f);
glm::mat4 machineRotate(1.0f);

glm::mat4 arm1Rotate(1.0f);
glm::mat4 arm2Rotate(1.0f);

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

glm::vec3 cameraPos = glm::vec3(3.0f, 3.0f, 10.0f);
glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

short isMachineMove = false;
short isMachineRotate = false;
short isArmRotate = false;

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

	box1Scale = glm::scale(glm::mat4(1.0f), glm::vec3(10.0f, 1.0f, 10.0f));
	box1Translate = glm::translate(glm::mat4(1.0f), glm::vec3(-5.0f, -1.0f, -5.0f));

	box2Scale = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 0.3f, 1.0f));

	box3Scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.7f, 0.4f, 0.7f));
	box3Translate = glm::translate(glm::mat4(1.0f), glm::vec3(0.15f, 0.3f, 0.15f));

	box4Scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.2f, 0.7f, 0.2f));
	box4Translate = glm::translate(glm::mat4(1.0f), glm::vec3(0.25f, 0.6f, 0.5f));

	box5Scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.2f, 0.7f, 0.2f));
	box5Translate = glm::translate(glm::mat4(1.0f), glm::vec3(0.6f, 0.6f, 0.5f));

	transformLocation = glGetUniformLocation(shaderID, "modelTransform");

	projection = glm::perspective(glm::radians(45.0f), (float)GLUT_SCREEN_WIDTH / (float)GLUT_SCREEN_HEIGHT, 0.1f, 50.0f);
	projectionLocation = glGetUniformLocation(shaderID, "projectionTransform");

	view = glm::lookAt(cameraPos, cameraDirection, cameraUp);
	viewLocation = glGetUniformLocation(shaderID, "viewTransform");

	glutMainLoop();
}

GLvoid drawScene() {
	static glm::mat4 tmp(1.0f);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shaderID);

	glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

	tmp = box1Translate *  box1Scale;
	glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(tmp));
	glBindVertexArray(VAO[0]);	// 삼각형의 속성이 있는 VAO[0] 사용
	glDrawArrays(GL_TRIANGLES, 0, 36);

	tmp = machineTranslate *  box2Translate * box2Scale;
	glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(tmp));
	glBindVertexArray(VAO[1]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	tmp = machineTranslate * box3Translate * glm::translate(glm::mat4(1.0f), glm::vec3(0.35f, 0.0f, 0.35f)) * machineRotate * glm::translate(glm::mat4(1.0f), glm::vec3(-0.35f, 0.0f, -0.35f)) * box3Scale;
	glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(tmp));
	glBindVertexArray(VAO[2]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	tmp = machineTranslate * box4Translate * glm::translate(glm::mat4(1.0f), glm::vec3(0.275f, 0.0f, 0.1f)) * machineRotate * glm::translate(glm::mat4(1.0f), glm::vec3(-0.275f, 0.0f, -0.1f)) * arm1Rotate * box4Scale;
	glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(tmp));
	glBindVertexArray(VAO[3]);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	tmp = machineTranslate * box5Translate * glm::translate(glm::mat4(1.0f), glm::vec3(-0.075f, 0.0f, 0.1f)) * machineRotate * glm::translate(glm::mat4(1.0f), glm::vec3(0.075f, 0.0f, -0.1f)) * arm2Rotate * box5Scale;
	glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(tmp));
	glDrawArrays(GL_TRIANGLES, 0, 36);

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

	glBufferData(GL_ARRAY_BUFFER, sizeof(Cube1), Cube1, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(VAO[1]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);

	glBufferData(GL_ARRAY_BUFFER, sizeof(Cube2), Cube2, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(VAO[2]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);

	glBufferData(GL_ARRAY_BUFFER, sizeof(Cube3), Cube3, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(VAO[3]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[3]);

	glBufferData(GL_ARRAY_BUFFER, sizeof(Cube4), Cube4, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
}


float Machinemove = 0.0f;
float MachineR = 0.0f;
float ArmR = 0.0f;

GLvoid Keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'b':
		isMachineMove = 1;
		break;
	case 'B':
		isMachineMove = 2;
		break;
	case 'm':
		isMachineRotate = 1;
		break;
	case 'M':
		isMachineRotate = 2;
		break;
	case 's':
	case 'S':
		isMachineMove = 0;
		isMachineRotate = 0;
		break;
	case 't':
		isArmRotate = 1;
		break;
	case 'T':
		isArmRotate = 2;
		break;
	case 'c':
	case 'C':
		isMachineMove = 0;
		isMachineRotate = 0;
		isArmRotate = 0;
		Machinemove = 0;
		MachineR = 0;
		machineTranslate = glm::mat4(1.0f);
		machineRotate = glm::mat4(1.0f);
		arm1Rotate = glm::mat4(1.0f);
		arm2Rotate = glm::mat4(1.0f);
		ArmR = 0;
		break;
	case 'z':
		view = glm::translate(view, glm::vec3(0.0f, 0.0f, 0.1f));
		break;
	case 'Z':
		view = glm::translate(view, glm::vec3(0.0f, 0.0f, -0.1f));
		break;
	}
	glutPostRedisplay();
}

GLvoid Timer(int value) {

	if (isMachineMove == 1) {
		machineTranslate = glm::translate(machineTranslate, glm::vec3(0.02f, 0.0f, 0.0f));
		Machinemove += 0.02f;
		if (Machinemove > 4.0f)
			isMachineMove = 0;
	}
	if (isMachineMove == 2) {
		machineTranslate = glm::translate(machineTranslate, glm::vec3(-0.02f, 0.0f, 0.0f));
		Machinemove -= 0.02f;
		if (Machinemove < -4.0f)
			isMachineMove = 0;
	}
	if (isMachineRotate == 1) {
		machineRotate = glm::rotate(machineRotate, glm::radians(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		MachineR += 1.0f;
		if (MachineR > 90.0f)
			isMachineRotate = 0;
	}
	if (isMachineRotate == 2) {
		machineRotate = glm::rotate(machineRotate, glm::radians(-1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		MachineR -= 1.0f;
		if (MachineR < -90.0f)
			isMachineRotate = 0;
	}
	if (isArmRotate == 1) {
		arm1Rotate = glm::rotate(arm1Rotate, glm::radians(0.5f), glm::vec3(1.0f, 0.0f, 0.0f));
		arm2Rotate = glm::rotate(arm2Rotate, glm::radians(-0.5f), glm::vec3(1.0f, 0.0f, 0.0f));
		ArmR += 0.5f;
		if (ArmR > 45.0f)
			isArmRotate = 0;
	}
	if (isArmRotate == 2) {
		arm1Rotate = glm::rotate(arm1Rotate, glm::radians(-0.5f), glm::vec3(1.0f, 0.0f, 0.0f));
		arm2Rotate = glm::rotate(arm2Rotate, glm::radians(0.5f), glm::vec3(1.0f, 0.0f, 0.0f));
		ArmR -= 0.5f;
		if (ArmR < -45.0f)
			isArmRotate = 0;
	}

	glutPostRedisplay();
	glutTimerFunc(30, Timer, 1);
}
