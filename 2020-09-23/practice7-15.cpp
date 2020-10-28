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
	vertexSource = filetobuf("vertex7-15.glsl");

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
	fragmentsource = filetobuf("fragment7-15.glsl");
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

glm::mat4 transform1(1.0f);
glm::mat4 transform2(1.0f);
glm::mat4 transform3(1.0f);

glm::mat4 transformMatrix1(1.0f);
glm::mat4 tx1(1.0f);
glm::mat4 ty1(1.0f);
glm::mat4 tz1(1.0f);
glm::mat4 rx1(1.0f);
glm::mat4 ry1(1.0f);
glm::mat4 s1(1.0f);

glm::mat4 MtransformMatrix2(1.0f);
glm::mat4 transformMatrix2(1.0f);
glm::mat4 ttx2(1.0f);
glm::mat4 tx2(1.0f);
glm::mat4 ty2(1.0f);
glm::mat4 tz2(1.0f);
glm::mat4 rry2(1.0f);
glm::mat4 ry2(1.0f);


glm::mat4 MtransformMatrix3(1.0f);
glm::mat4 transformMatrix3(1.0f);
glm::mat4 ttx3(1.0f);
glm::mat4 tx3(1.0f);
glm::mat4 ty3(1.0f);
glm::mat4 tz3(1.0f);
glm::mat4 rz3(1.0f);
glm::mat4 rry3(1.0f);
glm::mat4 ry3(1.0f);

glm::mat4 MtransformMatrix4(1.0f);
glm::mat4 transformMatrix4(1.0f);
glm::mat4 ttx4(1.0f);
glm::mat4 tx4(1.0f);
glm::mat4 ty4(1.0f);
glm::mat4 tz4(1.0f);
glm::mat4 rz4(1.0f);
glm::mat4 rry4(1.0f);
glm::mat4 ry4(1.0f);

glm::mat4 Allt(1.0f);
glm::mat4 Allr(1.0f);

glm::mat4 TransTemp(1.0f);

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

glm::mat4 projection = glm::mat4(1.0f);
unsigned int projectionLocation;
glm::mat4 view = glm::mat4(1.0f);
unsigned int viewLocation;

glm::vec3 cameraPos = glm::vec3(0.0f, 1.0f, 4.0f);
glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

void main(int argc, char** argv) {
	width = 600;
	height = 600;

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(width, height);
	glutCreateWindow("practice15");

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

	transformLocation = glGetUniformLocation(shaderID, "modelTransform");

	tx1 = glm::translate(tx1, glm::vec3(0.25f, 0.0f, 0.0f));
	ty1 = glm::translate(ty1, glm::vec3(0.0f, -0.25f, 0.0f));
	tz1 = glm::translate(tz1, glm::vec3(0.0f, 0.0f, -0.25f));
	s1 = glm::scale(s1, glm::vec3(0.5f, 0.5f, 0.5f));

	ttx2 = glm::translate(ttx2, glm::vec3(-0.3f, 0.0f, 0.0f));
	tx2 = glm::translate(tx2, glm::vec3(-1.0f, 0.0f, 0.0f));
	ty2 = glm::translate(ty2, glm::vec3(0.0f, 0.0f, 0.0f));
	tz2 = glm::translate(tz2, glm::vec3(0.0f, 0.0f, 0.0f));
	rry2 = glm::rotate(rry2, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	ry2 = glm::rotate(ry2, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	transformMatrix2 = ry2 * tx2 * ty2 * tz2;
	MtransformMatrix2 = transformMatrix2 * rry2 * ttx2;

	ttx3 = glm::translate(ttx3, glm::vec3(-0.3f, 0.0f, 0.0f));
	tx3 = glm::translate(tx3, glm::vec3(-1.0f, 0.0f, 0.0f));
	ty3 = glm::translate(ty3, glm::vec3(0.0f, 0.0f, 0.0f));
	tz3 = glm::translate(tz3, glm::vec3(0.0f, 0.0f, 0.0f));
	rry3 = glm::rotate(rry3, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	ry3 = glm::rotate(ry3, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	rz3 = glm::rotate(ry3, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	transformMatrix3 = rz3 * ry3 * tx3 * ty3 * tz3;
	MtransformMatrix3 = transformMatrix3 * rry3 * ttx3;

	ttx4 = glm::translate(ttx4, glm::vec3(-0.3f, 0.0f, 0.0f));
	tx4 = glm::translate(tx4, glm::vec3(-1.0f, 0.0f, 0.0f));
	ty4 = glm::translate(ty4, glm::vec3(0.0f, 0.0f, 0.0f));
	tz4 = glm::translate(tz4, glm::vec3(0.0f, 0.0f, 0.0f));
	rry4 = glm::rotate(rry4, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	ry4 = glm::rotate(ry4, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	rz4 = glm::rotate(ry4, glm::radians(-45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	transformMatrix4 = rz4 * ry4 * tx4 * ty4 * tz4;
	MtransformMatrix4 = transformMatrix4 * rry4 * ttx4;

	transform2 = glm::rotate(transform2, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	transform3 = glm::rotate(transform3, glm::radians(-45.0f), glm::vec3(0.0f, 0.0f, 1.0f));

	qobj = gluNewQuadric();
	gluQuadricDrawStyle(qobj, GLU_FILL);
	
	projection = glm::perspective(glm::radians(45.0f), (float)GLUT_SCREEN_WIDTH / (float)GLUT_SCREEN_HEIGHT, 0.1f, 100.0f);
	projectionLocation =  glGetUniformLocation(shaderID, "projectionTransform");

	view = glm::lookAt(cameraPos, cameraDirection, cameraUp);
	viewLocation = glGetUniformLocation(shaderID, "viewTransform");

	glutMainLoop();
}


GLvoid drawScene() {
	static float x, y, z;
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shaderID);

	glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

	transformMatrix1 = Allt * Allr;
	glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(transformMatrix1));
	gluSphere(qobj, 0.2, 20, 20);

	transformMatrix2 = Allt * Allr * ry2 * tx2 * ty2 * tz2;
	glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(transformMatrix2));
	gluSphere(qobj, 0.1, 10, 10);

	glBegin(GL_LINE_STRIP);
	for (float angle = 0.0f; angle <= (2.0f * 3.1415f) * 2.0f; angle += 0.1f)
	{
		x = 0.3f * cos(angle);
		z = 0.3f * sin(angle);
		glVertex3f(x, 0.0f, z);
	}
	glEnd();

	MtransformMatrix2 = transformMatrix2 * rry2 * ttx2;
	glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(MtransformMatrix2));
	gluSphere(qobj, 0.02, 10, 10);

	transformMatrix3 = Allt * Allr * rz3 * ry3 * tx3 * ty3 * tz3;
	glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(transformMatrix3));
	gluSphere(qobj, 0.1, 10, 10);

	glBegin(GL_LINE_STRIP);
	for (float angle = 0.0f; angle <= (2.0f * 3.1415f) * 2.0f; angle += 0.1f)
	{
		x = 0.3f * cos(angle);
		z = 0.3f * sin(angle);
		glVertex3f(x, 0.0f, z);
	}
	glEnd();

	MtransformMatrix3 = transformMatrix3 * rry3 * ttx3;
	glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(MtransformMatrix3));
	gluSphere(qobj, 0.02, 10, 10);

	transformMatrix4 = Allt * Allr * rz4 * ry4 * tx4 * ty4 * tz4;
	glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(transformMatrix4));
	gluSphere(qobj, 0.1, 10, 10);

	glBegin(GL_LINE_STRIP);
	for (float angle = 0.0f; angle <= (2.0f * 3.1415f) * 2.0f; angle += 0.1f)
	{
		x = 0.3f * cos(angle);
		z = 0.3f * sin(angle);
		glVertex3f(x, 0.0f, z);
	}
	glEnd();

	MtransformMatrix4 = transformMatrix4 * rry4 * ttx4;
	glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(MtransformMatrix4));
	gluSphere(qobj, 0.02, 10, 10);

	TransTemp = Allt * Allr * transform1;
	glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(TransTemp));
	glBegin(GL_LINE_STRIP);
	for (float angle = 0.0f; angle <= (2.0f * 3.1415f) * 2.0f; angle += 0.1f)
	{
		x = 1.0f * cos(angle);
		z = 1.0f * sin(angle);
		glVertex3f(x, 0.0f, z);
	}
	glEnd();

	TransTemp = Allt * Allr * transform2;
	glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(TransTemp));
	glBegin(GL_LINE_STRIP);
	for (float angle = 0.0f; angle <= (2.0f * 3.1415f) * 2.0f; angle += 0.1f)
	{
		x = 1.0f * cos(angle);
		z = 1.0f * sin(angle);
		glVertex3f(x, 0.0f, z);
	}
	glEnd();

	TransTemp = Allt * Allr * transform3;
	glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(TransTemp));
	glBegin(GL_LINE_STRIP);
	for (float angle = 0.0f; angle <= (2.0f * 3.1415f) * 2.0f; angle += 0.1f)
	{
		x = 1.0f * cos(angle);
		z = 1.0f * sin(angle);
		glVertex3f(x, 0.0f, z);
	}
	glEnd();
	
	glutSwapBuffers();
}

GLvoid Reshape(int w, int h) {							//---콜백함수 : 다시 그리기 콜백 함수
	glViewport(0, 0, w, h);
}

GLvoid InitBuffer() {
}


GLvoid Keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'w':
		Allt = glm::translate(Allt, glm::vec3(0.0f, 0.1f, 0.0f));
		break;
	case 'a':
		Allt = glm::translate(Allt, glm::vec3(-0.1f, 0.0f, 0.0f));
		break;
	case 's':
		Allt = glm::translate(Allt, glm::vec3(0.0f, -0.1f, 0.0f));
		break;
	case 'd':
		Allt = glm::translate(Allt, glm::vec3(0.1f, 0.0f, 0.0f));
		break;
	case 'z':
		Allt = glm::translate(Allt, glm::vec3(0.0f, 0.0f, 0.1f));
		break;
	case 'x':
		Allt = glm::translate(Allt, glm::vec3(0.0f, 0.0f, -0.1f));
		break;
	case 'y':
		rotateY = 1;
		break;
	case 'Y':
		rotateY = 2;
		break;
	case 'm':
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		break;
	case 'M':
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
	ry2 = glm::rotate(ry2, glm::radians(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	rry2 = glm::rotate(rry2, glm::radians(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	ry3 = glm::rotate(ry3, glm::radians(2.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	rry3 = glm::rotate(rry3, glm::radians(-1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	ry4 = glm::rotate(ry4, glm::radians(1.5f), glm::vec3(0.0f, 1.0f, 0.0f));
	rry4 = glm::rotate(rry4, glm::radians(3.0f), glm::vec3(0.0f, 1.0f, 0.0f));


	if (rotateY == 1) {
		Allr = glm::rotate(Allr, glm::radians(1.7f), glm::vec3(0.0f, 1.0f, 0.0f));
	}
	else if (rotateY == 2) {
		Allr = glm::rotate(Allr, glm::radians(-1.7f), glm::vec3(0.0f, 1.0f, 0.0f));
	}

	glutPostRedisplay();
	glutTimerFunc(30, Timer, 1);
}
