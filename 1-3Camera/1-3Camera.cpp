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
#define STB_IMAGE_IMPLEMENTATION
#include"stb_image.h"


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

GLuint VAO[2], VBO[2];

int direction = UP;
int old_t;

unsigned int texture;
const char* texturename = "bg1.bmp";

Robot robot1(-8.0f, -8.0f);
Robot robot2(8.0f, -8.0f);
Robot robot3(-8.0f, 8.0f);
Robot robot4(8.0f, 8.0f);
Player player;

float clamp(float n, float min, float max) {
	if (n < min)
		return min;
	else if (n > max)
		return max;
	return n;
}

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

GLuint shaderID;

void InitTexture() {
	int widthImage, heightImage, numberOfChannel;

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(texturename, &widthImage, &heightImage, &numberOfChannel, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, widthImage, heightImage, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	stbi_image_free(data);

	glUseProgram(shaderID);
	int tLocation = glGetUniformLocation(shaderID, "texture1");
	glUniform1i(tLocation, 0);
}

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

glm::vec3 thirdCameraPos = glm::vec3(0.0f, 10.0f, 25.0f);
glm::vec3 thirdCameraDirection = glm::vec3(0.0f, 0.0f, 0.0f);

glm::vec3 firstCameraPos = glm::vec3(0.0f, 2.0f, 0.0f);
glm::vec3 firstCameraDirection = glm::vec3(1.0f, 2.0f, 0.0f);

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

	view = glm::lookAt(thirdCameraPos, thirdCameraDirection, cameraUp);
	viewLocation = glGetUniformLocation(shaderID, "g_view");

	isTransLocation = glGetUniformLocation(shaderID, "Trans");
	isTextureLocation = glGetUniformLocation(shaderID, "is_texture");

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	InitTexture();

	cout << "###조작법###" << endl << "w/a/s/d : 이동" << endl << "y/Y : 카메라 자전" << endl << "c/C : 카메라 화면 중점 기준으로 회전"
		<< endl << "r : 카메라 값 초기화" << endl << "q : 프로그램 종료" << endl << "1/3 : 1인칭 시점 / 3인칭 시점" << endl;

	glutMainLoop();
}

GLvoid drawScene() {
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shaderID);
	glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);
	if (player.isThirdView)
		view = glm::lookAt(thirdCameraPos, thirdCameraDirection, cameraUp);
	else {
		glm::vec4 dirTmp = glm::translate(glm::mat4(1.0f), glm::vec3(player.x, 0.0f, player.z)) * glm::rotate(glm::mat4(1.0f), glm::radians(player.degree), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(firstCameraDirection, 1.0f);
		glm::vec4 posTmp = glm::translate(glm::mat4(1.0f), glm::vec3(player.x, 0.0f, player.z)) * glm::vec4(firstCameraPos, 1.0f);
		view = glm::lookAt(glm::vec3(posTmp), glm::vec3(dirTmp), cameraUp);
	}
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));

	// 배경 그리기
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	glUniform1i(isTransLocation, 0);
	glUniform1i(isTextureLocation, 1);
	glBindVertexArray(VAO[1]);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	// 로봇 그리기
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glBindVertexArray(VAO[0]);
	glUniform1i(isTransLocation, 1);
	glUniform1i(isTextureLocation, 0);

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
	glGenVertexArrays(2, VAO);
	glGenBuffers(2, VBO);

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

	// 백그라운드 VAO
	glBindVertexArray(VAO[1]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(BackGround), BackGround, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
}

GLvoid Keyboard(unsigned char key, int x, int y) {
	glm::vec4 tmp = glm::vec4(player.x, 0.0f, player.z, 1.0f);
	switch (key) {
	case 'w':
		player.x += cosf(glm::radians(player.degree)) * 0.5f;
		player.z -= sinf(glm::radians(player.degree)) * 0.5f;
		break;
	case 's':
		player.x -= cosf(glm::radians(player.degree)) * 0.5f;
		player.z += sinf(glm::radians(player.degree)) * 0.5f;
		break;
	case 'a':
		player.x -= sinf(glm::radians(player.degree)) * 0.5f;
		player.z -= cosf(glm::radians(player.degree)) * 0.5f;
		break;
	case 'd':
		player.x += sinf(glm::radians(player.degree)) * 0.5f;
		player.z += cosf(glm::radians(player.degree)) * 0.5f;
		break;
	case '1':
		player.isThirdView = false;
		break;
	case '3':
		player.isThirdView = true;
		break;
	case 'y':
		player.degree += 5.0f;
		break;
	case 'Y':
		player.degree -= 5.0f;
		break;
	case 'c':
		tmp = glm::rotate(glm::mat4(1.0f), glm::radians(5.0f), glm::vec3(0.0f, 1.0f, 0.0f)) * tmp;
		player.x = tmp.x;
		player.z = tmp.z;
		break;
	case 'C':
		tmp = glm::rotate(glm::mat4(1.0f), glm::radians(-5.0f), glm::vec3(0.0f, 1.0f, 0.0f)) * tmp;
		player.x = tmp.x;
		player.z = tmp.z;
		break;
	case 'r':
		player.x = 0.0f;
		player.z = 0.0f;
		player.degree = 90.0f;
		break;
	case 'q':
	case 'Q':
		glutLeaveMainLoop();
		break;
	}
	player.x = clamp(player.x, -14.0f, 14.0f);
	player.z = clamp(player.z, -14.0f, 14.0f);
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
