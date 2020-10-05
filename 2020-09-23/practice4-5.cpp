#include<stdlib.h>
#include<stdio.h>
#include<iostream>
#include<random>
#include<list>
#include<gl/glew.h>
#include<gl/freeglut.h>
#include<gl/freeglut_ext.h>

#define UP 1
#define DOWN 2
#define LEFT 3
#define RIGHT 4
GLvoid drawScene();
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);

GLvoid Timer(int value);
GLvoid InitBuffer();
GLvoid InitShader();
std::random_device rd;
std::default_random_engine dre(rd());
std::uniform_real_distribution<float> urd(0.0f, 1.0f);

GLuint VAO, VBO[2];
bool drawMode = 0;
bool isMove = 0;

int direction[4] = { dre() % 4 + 1, dre() % 4 + 1, dre() % 4 + 1, dre() % 4 + 1 };

class Coord {
public:
	GLfloat x = 0, y = 0;
};

Coord tris[4] = { {-0.5, -0.5}, {0.5, -0.5}, {-0.5, 0.5}, {0.5, 0.5}, };

int cnt = 0;

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

const GLfloat ref_triShape[3][3] = {
   {-0.125, -0.125 , 0.0},
   {0.125, -0.125, 0.0},
   {0.0, 0.125, 0.0}
};

GLfloat triShape[4][3][3] = {
	{
   {-0.125 - 0.25, -0.125 + 0.25, 0.0},
   {0.125 - 0.25, -0.125 + 0.25, 0.0},
   {0.0 - 0.25, 0.125 + 0.25, 0.0}
	},
	{
   {-0.125 + 0.25, -0.125 + 0.25, 0.0},
   {0.125 + 0.25, -0.125 + 0.25, 0.0},
   {0.0 + 0.25, 0.125 + 0.25, 0.0}
	},
	{
   {-0.125 - 0.25, -0.125 - 0.25, 0.0},
   {0.125 - 0.25, -0.125 - 0.25, 0.0},
   {0.0 - 0.25, 0.125 - 0.25, 0.0}
	},
	{
   {-0.125 + 0.25, -0.125 - 0.25, 0.0},
   {0.125 + 0.25, -0.125 - 0.25, 0.0},
   {0.0 + 0.25, 0.125 - 0.25, 0.0}
	},
};

GLfloat colors[4][3][3] = {
   {
   {1.0, 0.0, 0.0},
   {1.0, 0.0, 0.0},
   {1.0, 0.0, 0.0}
   },
	{
   {1.0, 1.0, 0.0},
   {1.0, 1.0, 0.0},
   {1.0, 1.0, 0.0}
   },
	{
   {0.0, 1.0, 0.0},
   {0.0, 1.0, 0.0},
   {0.0, 1.0, 0.0}
   },
	{
   {1.0, 0.0, 1.0},
   {1.0, 0.0, 1.0},
   {1.0, 0.0, 1.0}
   },
};


int width;
int height;

using namespace std;

GLuint vertexShader;
GLuint fragmentShader;


void make_vertexShaders() {
	// 버텍스 세이더 읽어 저장하고 컴파일 하기
	GLchar* vertexSource;
	vertexSource = filetobuf("vertex1.glsl");

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
	fragmentsource = filetobuf("fragment1.glsl");
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
	width = 500;
	height = 500;

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(width, height);
	glutCreateWindow("practice5");

	glewExperimental = GL_TRUE;
	glewInit();

	InitShader();
	InitBuffer();


	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);

	glutMainLoop();
}

GLvoid drawScene() {
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shaderID);

	glBindVertexArray(VAO);

	glDrawArrays(GL_TRIANGLES, 0, 3);
	glDrawArrays(GL_TRIANGLES, 3, 3);
	glDrawArrays(GL_TRIANGLES, 6, 3);
	glDrawArrays(GL_TRIANGLES, 9, 3);

	glutSwapBuffers();
}

GLvoid Reshape(int w, int h) {							//---콜백함수 : 다시 그리기 콜백 함수
	glViewport(0, 0, w, h);
}

GLvoid InitBuffer() {
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(2, VBO);
	//1번째 VBO활성화, 바인드, 좌표값 저장
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, 36 * sizeof(GLfloat), triShape, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, 36 * sizeof(GLfloat), colors, GL_STATIC_DRAW);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(1);

}

GLvoid Keyboard(unsigned char key, int x, int y) {
	float rt, gt, bt;
	switch (key) {
	case 'f':
		if(drawMode == 0)
			glPolygonMode(GL_FRONT, GL_LINE);
		else
			glPolygonMode(GL_FRONT, GL_FILL);
		drawMode = (drawMode + 1) % 2;
		break;
	case 'm':
		isMove = true;
		glutTimerFunc(100, Timer, 1);
		break;
	case 's':
		isMove = false;
		break;
	case 'c':
		for (int i = 0; i < 4; ++i) {
			rt = urd(dre);
			gt = urd(dre);
			bt = urd(dre);
			colors[i][0][0] = colors[i][1][0] = colors[i][2][0] = rt;
			colors[i][0][1] = colors[i][1][1] = colors[i][2][1] = gt;
			colors[i][0][2] = colors[i][1][2] = colors[i][2][2] = bt;
		}
		glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
		glBufferData(GL_ARRAY_BUFFER, 36 * sizeof(GLfloat), colors, GL_STATIC_DRAW);

		break;
	case 'q':
		glutLeaveMainLoop();
		break;
	}
	glutPostRedisplay();
}

GLvoid Timer(int value) {
	for (int i = 0; i < 4; ++i) {
		switch (direction[i]) {
		case UP:
			if (triShape[i][2][1] >= 1.0)
				direction[i] = LEFT;
			else
				for (int j = 0; j < 3; ++j) {
					triShape[i][j][1] += 0.05;
					cout << triShape[i][j][1] << endl;
				}
			break;
		case DOWN:
			if (triShape[i][0][1] <= -1.0)
				direction[i] = RIGHT;
			else
				for (int j = 0; j < 3; ++j) {
					triShape[i][j][1] -= 0.05;
					cout << triShape[i][j][1] << endl;
				}
			break;
		case LEFT:
			if (triShape[i][0][0] <= -1.0)
				direction[i] = DOWN;
			else
				for (int j = 0; j < 3; ++j) {
					triShape[i][j][0] -= 0.05;
					cout << triShape[i][j][0] << endl;
				}
			break;
		case RIGHT:
			if (triShape[i][1][0] >= 1.0)
				direction[i] = UP;
			else
				for (int j = 0; j < 3; ++j) {
					triShape[i][j][0] += 0.05;
					cout << triShape[i][j][0] << endl;
				}
			break;
		}
	}
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, 36 * sizeof(GLfloat), triShape, GL_STATIC_DRAW);
	glutPostRedisplay();
	if (isMove)glutTimerFunc(30, Timer, 1);
}