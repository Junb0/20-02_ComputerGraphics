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
#include"robot.hpp"

#define UP 1
#define DOWN 2
#define LEFT 3
#define RIGHT 4

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
GLvoid Timer(int value);
GLvoid Keyboard(unsigned char key, int x, int y);

GLuint VAO[9], VBO[9];

random_device rd;
default_random_engine dre(rd());
uniform_real_distribution<> urd1(-0.7f, 0.7f);
uniform_real_distribution<> urd2(0.005f, 0.02f);
uniform_real_distribution<> urdColor(0.0f, 1.0f);

int direction = UP;
int old_t;

Robot robot1(-8.0f, -8.0f);
Robot robot2(8.0f, -8.0f);
Robot robot3(-8.0f, 8.0f);
Robot robot4(8.0f, 8.0f);
Player player;

char* filetobuf(const char* file) {
	FILE* fptr;
	long length;
	char* buf;
	fopen_s(&fptr, file, "rb"); // Open file for reading
	if (!fptr) // Return NULL on failure
		return NULL;
	fseek(fptr, 0, SEEK_END); // Seek to the end of the file
	length = ftell(fptr); // Find out how many bytes into the file we are
	buf = (char*)malloc(length + 1); // Allocate a buffer for the entire length of the file and a null terminator
	fseek(fptr, 0, SEEK_SET); // Go back to the beginning of the file
	fread(buf, length, 1, fptr); // Read the contents of the file in to the buffer
	fclose(fptr); // Close the file
	buf[length] = 0; // Null terminator
	return buf; // Return the buffer
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

void make_vertexShaders()
{
	char* vertexsource = filetobuf("vertex.glsl");

	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, (const GLchar**)&vertexsource, 0);
	glCompileShader(vertexShader);

	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
		cerr << "ERROR: vertex shader error\n" << errorLog << endl;
	}
}

void make_fragmentShaders()
{
	char* fragmentsource = filetobuf("fragment.glsl");

	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, (const GLchar**)&fragmentsource, 0);
	glCompileShader(fragmentShader);

	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
		cerr << "ERROR: fragment shader error\n" << errorLog << endl;
	}
}

GLuint shaderID;
glm::mat4 transformMatrix(1.0f);
glm::mat4 cameraRotate(1.0f);

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

glm::vec3 cameraPos = glm::vec3(0.0f, 10.0f, 25.0f);
glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

short isMachineMove = false;
short isMachineRotate = false;
short isArmRotate = 1;

unsigned int isTransLocation;
unsigned int isTextureLocation;

void main(int argc, char** argv) {
	width = 600;
	height = 600;

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(width, height);
	glutCreateWindow("1/3Camera");

	glewExperimental = GL_TRUE;
	glewInit();

	InitShader();
	InitBuffer();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glutKeyboardFunc(Keyboard);
	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutTimerFunc(30, Timer, 1);

	transformLocation = glGetUniformLocation(shaderID, "g_modelTransform");

	projection = glm::perspective(glm::radians(45.0f), (float)GLUT_SCREEN_WIDTH / (float)GLUT_SCREEN_HEIGHT, 0.1f, 500.0f);
	projectionLocation = glGetUniformLocation(shaderID, "g_projection");

	view = glm::lookAt(cameraPos, cameraDirection, cameraUp);
	viewLocation = glGetUniformLocation(shaderID, "g_view");

	isTransLocation = glGetUniformLocation(shaderID, "Trans");
	isTextureLocation = glGetUniformLocation(shaderID, "is_texture");

	glutMainLoop();
}

GLvoid drawScene() {
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shaderID);
	glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

	glBindVertexArray(VAO[0]);
	glUniform1i(isTransLocation, 1);
	glUniform1i(isTextureLocation, 0);

	// 로봇 그리기
	robot1.Draw(shaderID);
	robot2.Draw(shaderID);
	robot3.Draw(shaderID);
	robot4.Draw(shaderID);

	// 바닥 그리기
	DrawGround(shaderID);

	// 플레이어 그리기
	DrawPlayer(shaderID, player);

	glutSwapBuffers();
}

GLvoid Reshape(int w, int h) {							//---콜백함수 : 다시 그리기 콜백 함수
	glViewport(0, 0, w, h);
}

GLvoid InitBuffer() {
	glGenVertexArrays(1, VAO);
	glGenBuffers(1, VBO);

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
}

glm::mat4 cameraDegree(1.0f);

GLvoid Keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 's':
		if (player.z < 14.0f)
			player.z += 0.5f;
		break;
	case 'a':
		if (player.x > -14.0f)
			player.x -= 0.5f;
		break;
	case 'w':
		if (player.z > -14.0f)
			player.z -= 0.5f;
		break;
	case 'd':
		if (player.x < 14.0f)
			player.x += 0.5f;
		break;
	case 'q':
	case 'Q':
		glutLeaveMainLoop();
		break;
	}
	glutPostRedisplay();
}

GLvoid Timer(int value) {
	int t;
	float dt;
	t = glutGet(GLUT_ELAPSED_TIME);
	dt = (t - old_t) / 1000.0;
	old_t = t;

	robot1.Update(dt, player.x, player.z);
	robot2.Update(dt, player.x, player.z);
	robot3.Update(dt, player.x, player.z);
	robot4.Update(dt, player.x, player.z);
	glutPostRedisplay();
	glutTimerFunc(30, Timer, 1);
}
