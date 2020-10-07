#include<stdlib.h>
#include<stdio.h>
#include<iostream>
#include<random>
#include<vector>
#include<gl/glew.h>
#include<gl/freeglut.h>
#include<gl/freeglut_ext.h>
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


using namespace std;

GLvoid drawScene();
GLvoid Reshape(int w, int h);
GLvoid Timer(int value);
GLvoid InitBuffer();
GLvoid InitShader();
GLvoid UpdateBuffer();

GLuint VAO[2], VBO[2];

random_device rd;
default_random_engine dre(rd());
uniform_real_distribution<> urd1(-0.7f, 0.7f);
uniform_real_distribution<> urd2(0.005f, 0.02f);
uniform_real_distribution<> urdColor(0.0f, 1.0f);

float delta = 0.01f;
float deltaS = 0.001f;

int is1 = 1;	// 0 : 멈춤	1 : 가장자리로 이동	2 : 중앙으로 이동
int is2 = 1;	// 0 : 멈춤	1 : 점점 작아짐		2 : 점점 커짐
int is3 = 1;	// 0 : 멈춤	1 : 가장자리로 이동	2 : 중앙으로 이동
int is4 = 1;	// 0 : 멈춤	1 : 점점 작아짐		2 : 점점 커짐


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

GLfloat Triangle[12][3][2][3] = {
	{{{ -0.2f, 0.8f, 0.0f}, {0.0f, 0.0f, 1.0f}}, {{-0.2f, 0.2f, 0.0f}, {0.0f, 0.0f, 1.0f}}, {{-0.8f, 0.2f, 0.0f}, {0.0f, 0.0f, 1.0f}}},		// 좌상단 선 -> 삼각형
	{{{0.2f, 0.2f, 0.0f}, {1.0f, 1.0f, 0.0f}}, {{0.8f, 0.2f, 0.0f}, {1.0f, 1.0f, 0.0f}}, {{0.2f, 0.8f, 0.0f} , {1.0f, 1.0f, 0.0f}}},		// 우상단 삼각형 -> 사각형
	{{{0.2f, 0.8f, 0.0f}, {1.0f, 1.0f, 0.0f}}, {{0.8f, 0.8f, 0.0f}, {1.0f, 1.0f, 0.0f}}, {{0.8f, 0.2f, 0.0f} , {1.0f, 1.0f, 0.0f}}},
	{{{-0.5f, -0.2f, 0.0f}, {0.0f, 1.0f, 0.0f}}, {{-0.2f, -0.4f, 0.0f}, {0.0f, 1.0f, 0.0f}}, {{-0.8f, -0.4f, 0.0f} , {0.0f, 1.0f, 0.0f}}},			// 좌하단 사각형 -> 오각형
	{{{-0.6f, -0.8f, 0.0f}, {0.0f, 1.0f, 0.0f}}, {{-0.8f, -0.4f, 0.0f}, {0.0f, 1.0f, 0.0f}}, {{-0.5f, -0.8f, 0.0f} , {0.0f, 1.0f, 0.0f}}},
	{{{-0.4f, -0.8f, 0.0f}, {0.0f, 1.0f, 0.0f}}, {{-0.5f, -0.8f, 0.0f}, {0.0f, 1.0f, 0.0f}}, {{-0.2f, -0.4f, 0.0f} , {0.0f, 1.0f, 0.0f}}},
	{{{-0.5f, -0.8f, 0.0f}, {0.0f, 1.0f, 0.0f}}, {{-0.8f, -0.4f, 0.0f}, {0.0f, 1.0f, 0.0f}}, {{-0.2f, -0.4f, 0.0f} , {0.0f, 1.0f, 0.0f}}},

	{{{0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}}, {{0.3f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}}, {{0.5f, -0.3f, 0.0f} , {1.0f, 0.0f, 0.0f}}},			// 우하단 오각형 -> 점
	{{{0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}}, {{0.5f, -0.3f, 0.0f}, {1.0f, 0.0f, 0.0f}}, {{0.7f, -0.5f, 0.0f} , {1.0f, 0.0f, 0.0f}}},
	{{{0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}}, {{0.7f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}}, {{0.6f, -0.7f, 0.0f} , {1.0f, 0.0f, 0.0f}}},
	{{{0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}}, {{0.6f, -0.7f, 0.0f}, {1.0f, 0.0f, 0.0f}}, {{0.4f, -0.7f, 0.0f} , {1.0f, 0.0f, 0.0f}}},
	{{{0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}}, {{0.4f, -0.7f, 0.0f}, {1.0f, 0.0f, 0.0f}}, {{0.3f, -0.5f, 0.0f} , {1.0f, 0.0f, 0.0f}}}
};

GLfloat Lines[4][2][3] = {
	{{0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},
	{{0.0f, -1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},
	{{-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},
	{{1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},
};

int width;
int height;

GLuint vertexShader;
GLuint fragmentShader;

void make_vertexShaders() {
	// 버텍스 세이더 읽어 저장하고 컴파일 하기
	GLchar* vertexSource;
	vertexSource = filetobuf("vertex4-8.glsl");

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
	fragmentsource = filetobuf("fragment4-8.glsl");
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
	width = 800;
	height = 600;

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(width, height);
	glutCreateWindow("practice7");

	glewExperimental = GL_TRUE;
	glewInit();

	InitShader();
	InitBuffer();

	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutTimerFunc(30, Timer, 1);

	glutMainLoop();
}

GLvoid drawScene() {
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shaderID);

	glBindVertexArray(VAO[0]);	// 삼각형의 속성이 있는 VAO[0] 사용

	UpdateBuffer();
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glBindVertexArray(VAO[1]);
	glDrawArrays(GL_LINES, 0, 4);

	glutSwapBuffers();
}

GLvoid Reshape(int w, int h) {							//---콜백함수 : 다시 그리기 콜백 함수
	glViewport(0, 0, w, h);
}

GLvoid InitBuffer() {
	glGenVertexArrays(2, VAO);
	glGenBuffers(2, VBO);

	// 삼각형 VAO
	glBindVertexArray(VAO[0]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);

	glBufferData(GL_ARRAY_BUFFER, sizeof(Triangle), Triangle, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// 선 VAO
	glBindVertexArray(VAO[1]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);

	glBufferData(GL_ARRAY_BUFFER, sizeof(Lines), Lines, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
}

GLvoid Timer(int value) {

	if (is1 == 1) {
		Triangle[0][0][0][1] -= 0.01f;
		if (Triangle[0][0][0][1] <= 0.2f)
			is1 = 2;
	}
	if (is1 == 2) {
		Triangle[0][0][0][1] += 0.01f;
		if (Triangle[0][0][0][1] >= 0.8f)
			is1 = 1;
	}

	if (is2 == 1) {
		Triangle[2][1][0][1] -= 0.01f;
		Triangle[2][1][0][0] -= 0.01f;
		if (Triangle[2][1][0][1] < 0.4f)
			is2 = 2;
	}
	if (is2 == 2) {
		Triangle[2][1][0][1] += 0.01f;
		Triangle[2][1][0][0] += 0.01f;
		if (Triangle[2][1][0][1] > 0.8f)
			is2 = 1;
	}

	if (is3 == 1) {
		Triangle[3][0][0][1] -= 0.01f;
		Triangle[4][0][0][0] -= 0.01f;
		Triangle[5][0][0][0] += 0.01f;
		if (Triangle[3][0][0][1] <= -0.4f)
			is3 = 2;
	}
	if (is3 == 2) {
		Triangle[3][0][0][1] += 0.01f;
		Triangle[4][0][0][0] += 0.01f;
		Triangle[5][0][0][0] -= 0.01f;
		if (Triangle[3][0][0][1] >= -0.2f)
			is3 = 1;
	}

	if (is4 == 1) {
		Triangle[7][1][0][0] += 0.01f;
		Triangle[7][2][0][1] -= 0.01f;

		Triangle[8][1][0][1] -= 0.01f;
		Triangle[8][2][0][0] -= 0.01f;

		Triangle[9][1][0][0] -= 0.01f;
		Triangle[9][2][0][0] -= 0.005f;
		Triangle[9][2][0][1] += 0.01f;

		Triangle[10][1][0][0] -= 0.005f;
		Triangle[10][1][0][1] += 0.01f;
		Triangle[10][2][0][0] += 0.005f;
		Triangle[10][2][0][1] += 0.01f;

		Triangle[11][1][0][0] += 0.005f;
		Triangle[11][1][0][1] += 0.01f;
		Triangle[11][2][0][0] += 0.01f;
		if (Triangle[7][1][0][0] >= 0.49f)
			is4 = 2;
	}

	if (is4 == 2) {
		Triangle[7][1][0][0] -= 0.01f;
		Triangle[7][2][0][1] += 0.01f;

		Triangle[8][1][0][1] += 0.01f;
		Triangle[8][2][0][0] += 0.01f;

		Triangle[9][1][0][0] += 0.01f;
		Triangle[9][2][0][0] += 0.005f;
		Triangle[9][2][0][1] -= 0.01f;

		Triangle[10][1][0][0] += 0.005f;
		Triangle[10][1][0][1] -= 0.01f;
		Triangle[10][2][0][0] -= 0.005f;
		Triangle[10][2][0][1] -= 0.01f;

		Triangle[11][1][0][0] -= 0.005f;
		Triangle[11][1][0][1] -= 0.01f;
		Triangle[11][2][0][0] -= 0.01f;
		if (Triangle[7][1][0][0] <= 0.3f)
			is4 = 1;
	}

	glutPostRedisplay();
	glutTimerFunc(30, Timer, 1);
}

GLvoid UpdateBuffer() {
	glBindVertexArray(VAO[0]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Triangle), Triangle, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
}
