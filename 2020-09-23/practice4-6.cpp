#include<stdlib.h>
#include<stdio.h>
#include<iostream>
#include<random>
#include<list>
#include<gl/glew.h>
#include<gl/freeglut.h>
#include<gl/freeglut_ext.h>
#include<math.h>
#define VERTEXS 144
using namespace std;

GLvoid drawScene();
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid Mouse(int button, int state, int x, int y);

GLvoid Timer(int value);
GLvoid InitBuffer();
GLvoid InitShader();

GLvoid UpdateBuffer();

GLuint VAO, VBO[2];

random_device rd;
default_random_engine dre(rd());

class cir {
public:
	GLfloat x = 0, y = 0;
	float rad = 0;
	int type = dre() % 2;
};

std::list<cir> cirs;

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

GLfloat Circle[VERTEXS][3];

int width;
int height;
int DrawMode = GL_POINTS;

GLuint vertexShader;
GLuint fragmentShader;

void make_vertexShaders() {
	// 버텍스 세이더 읽어 저장하고 컴파일 하기
	GLchar* vertexSource;
	vertexSource = filetobuf("vertex4-6.glsl");

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
	fragmentsource = filetobuf("fragment4-6.glsl");
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
	width = 600;
	height = 600;

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(width, height);
	glutCreateWindow("practice6");

	glewExperimental = GL_TRUE;
	glewInit();

	InitShader();
	InitBuffer();

	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutMouseFunc(Mouse);
	glutKeyboardFunc(Keyboard);
	glutTimerFunc(10, Timer, 1);

	glutMainLoop();
}

GLvoid drawScene() {
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shaderID);

	glBindVertexArray(VAO);

	for (std::list<cir>::iterator iter = cirs.begin(); iter != cirs.end(); ++iter) {
		for (int i = 0; i < VERTEXS; ++i) {
			Circle[i][0] = (*iter).x + (*iter).rad * cos(2 * 3.141592 * ((float)i / VERTEXS));
			Circle[i][1] = (*iter).y + (*iter).rad * sin(2 * 3.141592 * ((float)i / VERTEXS));
			Circle[i][2] = 0.3f;
		}
		UpdateBuffer();
		glDrawArrays(DrawMode, 0, VERTEXS);
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

	glBufferData(GL_ARRAY_BUFFER, sizeof(Circle), Circle, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
}

GLvoid Mouse(int button, int state, int x, int y) {
	float f = 0;
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		if (cirs.size() < 10) {
			cirs.push_back({ (float)(x - (width / 2)) / (float)width * 2.0f, (-1.0f) * (float)(y - (height / 2)) / (float)height * 2.0f });
			std::cout << "new x : " << cirs.back().x << " y : " << cirs.back().y << std::endl;
		}
	}
	glutPostRedisplay();
}

GLvoid Timer(int value) {
	for (std::list<cir>::iterator iter = cirs.begin(); iter != cirs.end();) {
		(*iter).rad += 0.005f;
		if ((*iter).type == 0) {
			if ((*iter).rad > 1.0f)
				(*iter).rad = 0.0f;
			++iter;
		}
		else {
			if ((*iter).rad > 3.0f)
				iter = cirs.erase(iter);
		}
		
	}
	glutPostRedisplay();
	glutTimerFunc(10, Timer, 1);
}

GLvoid UpdateBuffer() {
	glBufferData(GL_ARRAY_BUFFER, sizeof(Circle), Circle, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
}

GLvoid Keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case '1':
		DrawMode = GL_POINTS;
		break;
	case '2':
		DrawMode = GL_LINES;
		break;
	}
	glutPostRedisplay();
}