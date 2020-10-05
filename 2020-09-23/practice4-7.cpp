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
#define TRIWIDTH 0.1f

using namespace std;

GLvoid drawScene();
GLvoid Reshape(int w, int h);

GLvoid Timer(int value);
GLvoid InitBuffer();
GLvoid InitShader();

GLvoid UpdateBuffer();

GLuint VAO, VBO[2];

random_device rd;
default_random_engine dre(rd());
uniform_real_distribution<> urd1(-0.7f, 0.7f);
uniform_real_distribution<> urd2(0.005f, 0.02f);

class tri {
public:
	GLfloat x = urd1(dre), y = urd1(dre);	// 삼각형의 pivot 좌표
	int direction = dre() % 4 + 1;
	GLfloat dx = urd2(dre), dy = urd2(dre);

	tri() {
		switch (direction) {
		case UP:
			if (dre() % 2 == 0)
				dx *= -1;
			break;
		case DOWN:
			dy *= -1;
			if (dre() % 2 == 0)
				dx *= -1;
			break;
		case LEFT:
			dx *= -1;
			if (dre() % 2 == 0)
				dy *= -1;
			break;
		case RIGHT:
			if (dre() % 2 == 0)
				dy *= -1;
			break;
		}
	}
};

std::vector<tri> tris(10);

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

GLfloat Triangle[3][3];

int width;
int height;
int DrawMode = GL_POINTS;

GLuint vertexShader;
GLuint fragmentShader;

void make_vertexShaders() {
	// 버텍스 세이더 읽어 저장하고 컴파일 하기
	GLchar* vertexSource;
	vertexSource = filetobuf("vertex4-7.glsl");

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
	fragmentsource = filetobuf("fragment4-7.glsl");
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
	width = 700;
	height = 700;

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
	glutTimerFunc(10, Timer, 1);

	glutMainLoop();
}

GLvoid drawScene() {
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shaderID);

	glBindVertexArray(VAO);

	for (std::vector<tri>::iterator iter = tris.begin(); iter != tris.end(); ++iter) {
		switch ((*iter).direction) {
		case UP:
			Triangle[0][0] = (*iter).x;
			Triangle[0][1] = (*iter).y + TRIHEIGHT / 2;
			Triangle[0][2] = 0.0f;
			Triangle[1][0] = (*iter).x + TRIWIDTH / 2;
			Triangle[1][1] = (*iter).y - TRIHEIGHT / 2;
			Triangle[1][2] = 0.0f;
			Triangle[2][0] = (*iter).x - TRIWIDTH / 2;
			Triangle[2][1] = (*iter).y - TRIHEIGHT / 2;
			Triangle[2][2] = 0.0f;
			break;
		case DOWN:
			Triangle[0][0] = (*iter).x - TRIWIDTH / 2;
			Triangle[0][1] = (*iter).y + TRIHEIGHT / 2;
			Triangle[0][2] = 0.0f;
			Triangle[1][0] = (*iter).x + TRIWIDTH / 2;
			Triangle[1][1] = (*iter).y + TRIHEIGHT / 2;
			Triangle[1][2] = 0.0f;
			Triangle[2][0] = (*iter).x;
			Triangle[2][1] = (*iter).y - TRIHEIGHT / 2;
			Triangle[2][2] = 0.0f;
			break;
		case LEFT:
			Triangle[0][0] = (*iter).x - TRIHEIGHT / 2;
			Triangle[0][1] = (*iter).y;
			Triangle[0][2] = 0.0f;
			Triangle[1][0] = (*iter).x + TRIHEIGHT / 2;
			Triangle[1][1] = (*iter).y + TRIWIDTH / 2;
			Triangle[1][2] = 0.0f;
			Triangle[2][0] = (*iter).x + TRIHEIGHT / 2;
			Triangle[2][1] = (*iter).y - TRIWIDTH / 2;
			Triangle[2][2] = 0.0f;
			break;
		case RIGHT:
			Triangle[0][0] = (*iter).x - TRIHEIGHT / 2;
			Triangle[0][1] = (*iter).y + TRIWIDTH / 2;
			Triangle[0][2] = 0.0f;
			Triangle[1][0] = (*iter).x + TRIHEIGHT / 2;
			Triangle[1][1] = (*iter).y;
			Triangle[1][2] = 0.0f;
			Triangle[2][0] = (*iter).x - TRIHEIGHT / 2;
			Triangle[2][1] = (*iter).y - TRIWIDTH / 2;
			Triangle[2][2] = 0.0f;
			break;
		}
		UpdateBuffer();
		glDrawArrays(GL_TRIANGLES, 0, 3);
	}

	glutSwapBuffers();
}

GLvoid Reshape(int w, int h) {							//---콜백함수 : 다시 그리기 콜백 함수
	glViewport(0, 0, w, h);
}

GLvoid InitBuffer() {
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);

	glBufferData(GL_ARRAY_BUFFER, sizeof(Triangle), Triangle, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);
}

GLvoid Timer(int value) {
	for (std::vector<tri>::iterator iter = tris.begin(); iter != tris.end(); ++iter) {
		(*iter).x += (*iter).dx;
		(*iter).y += (*iter).dy;
		switch ((*iter).direction) {
		case UP:
			if ((*iter).y + TRIHEIGHT / 2 > 1.0f) {
				(*iter).dy *= -1.0f;
				(*iter).direction = DOWN;
			}
			else if ((*iter).x + TRIWIDTH / 2 > 1.0f) {
				(*iter).dx *= -1.0f;
				(*iter).direction = LEFT;
			}
			else if ((*iter).x - TRIWIDTH / 2 < -1.0f) {
				(*iter).dx *= -1.0f;
				(*iter).direction = RIGHT;
			}
			break;
		case DOWN:
			if ((*iter).y - TRIHEIGHT / 2 < -1.0f) {
				(*iter).dy *= -1.0f;
				(*iter).direction = UP;
			}
			else if ((*iter).x + TRIWIDTH / 2 > 1.0f) {
				(*iter).dx *= -1.0f;
				(*iter).direction = LEFT;
			}
			else if ((*iter).x - TRIWIDTH / 2 < -1.0f) {
				(*iter).dx *= -1.0f;
				(*iter).direction = RIGHT;
			}
			break;
		case LEFT:
			if ((*iter).y + TRIWIDTH / 2 > 1.0f) {
				(*iter).dy *= -1.0f;
				(*iter).direction = DOWN;
			}
			else if ((*iter).y - TRIWIDTH / 2 < -1.0f) {
				(*iter).dy *= -1.0f;
				(*iter).direction = UP;
			}
			else if ((*iter).x - TRIHEIGHT / 2 < -1.0f) {
				(*iter).dx *= -1.0f;
				(*iter).direction = RIGHT;
			}
			break;
		case RIGHT:
			if ((*iter).y + TRIWIDTH / 2 > 1.0f) {
				(*iter).dy *= -1.0f;
				(*iter).direction = DOWN;
			}
			else if ((*iter).y - TRIWIDTH / 2 < -1.0f) {
				(*iter).dy *= -1.0f;
				(*iter).direction = UP;
			}
			else if ((*iter).x + TRIHEIGHT / 2 > 1.0f) {
				(*iter).dx *= -1.0f;
				(*iter).direction = LEFT;
			}
			break;
		}
	}
	glutPostRedisplay();
	glutTimerFunc(10, Timer, 1);
}

GLvoid UpdateBuffer() {
	glBufferData(GL_ARRAY_BUFFER, sizeof(Triangle), Triangle, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
}