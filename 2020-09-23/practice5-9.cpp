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
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid InitBuffer();
GLvoid InitShader();

GLvoid UpdateBuffer();

GLuint VAO, VBO;

random_device rd;
default_random_engine dre(rd());
uniform_real_distribution<> urd1(-0.7f, 0.7f);
uniform_real_distribution<> urd2(0.005f, 0.02f);
uniform_real_distribution<> urdColor(0.0f, 1.0f);

float delta = 0.01f;
float deltaS = 0.001f;

int isT = 0;	// 0 : ����	1 : �����ڸ��� �̵�	2 : �߾����� �̵�
int isS = 0;	// 0 : ����	1 : ���� �۾���		2 : ���� Ŀ��

int isTtmp = 1;
int isStmp = 1;

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

GLfloat Triangle[4][3][2][3] = {
	{{{TRIHEIGHT * -1.0f, TRIWIDTH / 2, 0.0f}, {urdColor(dre), urdColor(dre), urdColor(dre)}}, {{0.0f, 0.0f, 0.0f}, {urdColor(dre), urdColor(dre), urdColor(dre)}}, {{TRIHEIGHT * -1.0f, TRIWIDTH / 2 * -1.0f, 0.0f}, {urdColor(dre), urdColor(dre), urdColor(dre)}}},	// ���� �ﰢ��
	{{{TRIHEIGHT, TRIWIDTH / 2, 0.0f}, {urdColor(dre), urdColor(dre), urdColor(dre)}}, {{TRIHEIGHT, TRIWIDTH / 2 * -1.0f, 0.0f}, {urdColor(dre), urdColor(dre), urdColor(dre)}}, {{0.0f, 0.0f, 0.0f} , {urdColor(dre), urdColor(dre), urdColor(dre)}}},					// ������ �ﰢ��
	{{{0.0f, 0.0f, 0.0f}, {urdColor(dre), urdColor(dre), urdColor(dre)}}, {{TRIWIDTH / 2 * -1, TRIHEIGHT, 0.0f}, {urdColor(dre), urdColor(dre), urdColor(dre)}}, {{TRIWIDTH / 2, TRIHEIGHT, 0.0f}, {urdColor(dre), urdColor(dre), urdColor(dre)}}},	// ���� �ﰢ��
	{{{0.0f, 0.0f, 0.0f}, {urdColor(dre), urdColor(dre), urdColor(dre)}}, {{TRIWIDTH / 2, TRIHEIGHT * -1.0f, 0.0f}, {urdColor(dre), urdColor(dre), urdColor(dre)}}, {{TRIWIDTH / 2 * -1.0f, TRIHEIGHT * -1.0f, 0.0f}, {urdColor(dre), urdColor(dre), urdColor(dre)}}}	// �Ʒ��� �ﰢ��
};

int width;
int height;

GLuint vertexShader;
GLuint fragmentShader;

void make_vertexShaders() {
	// ���ؽ� ���̴� �о� �����ϰ� ������ �ϱ�
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
		cerr << "ERROR:vertex shader ������ ����\n" << errorLog << endl;
	}
}

void make_fragmentShaders() {
	GLchar* fragmentsource;
	fragmentsource = filetobuf("fragment4-8.glsl");
	// �����׸�Ʈ ���̴� �о� �����ϰ� �������ϱ�
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentsource, NULL);
	glCompileShader(fragmentShader);

	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (!result) {
		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
		cerr << "ERROR:fragment shader ������ ����\n" << errorLog << endl;
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
	glutTimerFunc(10, Timer, 1);
	glutKeyboardFunc(Keyboard);

	glutMainLoop();
}

GLvoid drawScene() {
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shaderID);

	glBindVertexArray(VAO);	// �ﰢ���� �Ӽ��� �ִ� VAO[0] ���
	
	UpdateBuffer();
	glDrawArrays(GL_TRIANGLES, 0, 12);

	glutSwapBuffers();
}

GLvoid Reshape(int w, int h) {							//---�ݹ��Լ� : �ٽ� �׸��� �ݹ� �Լ�
	glViewport(0, 0, w, h);
}

GLvoid InitBuffer() {
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	// �ﰢ�� VAO
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(Triangle), Triangle, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
}

GLvoid Timer(int value) {

	if (isT) {
		if (isT == 1) {
			for (int i = 0; i < 3; ++i) {
				Triangle[0][i][0][0] -= delta;
				Triangle[1][i][0][0] += delta;
				Triangle[2][i][0][1] += delta;
				Triangle[3][i][0][1] -= delta;
			}
			if (Triangle[1][0][0][0] >= 1.0f) {
				isT = 2;
			}
		}
		else if (isT == 2) {
			for (int i = 0; i < 3; ++i) {
				Triangle[0][i][0][0] += delta;
				Triangle[1][i][0][0] -= delta;
				Triangle[2][i][0][1] -= delta;
				Triangle[3][i][0][1] += delta;
			}
			if (Triangle[1][2][0][0] <= 0.0f) {
				isT = 1;
			}
		}
	}

	if (isS) {
		if (isS == 1) {
			Triangle[0][0][0][1] -= deltaS;
			Triangle[0][2][0][1] += deltaS;

			Triangle[1][0][0][1] -= deltaS;
			Triangle[1][1][0][1] += deltaS;

			Triangle[2][1][0][0] += deltaS;
			Triangle[2][2][0][0] -= deltaS;

			Triangle[3][1][0][0] -= deltaS;
			Triangle[3][2][0][0] += deltaS;

			if (Triangle[0][0][0][1] <= 0.02f) {
				isS = 2;
			}
		}
		else if (isS == 2) {
			Triangle[0][0][0][1] += deltaS;
			Triangle[0][2][0][1] -= deltaS;

			Triangle[1][0][0][1] += deltaS;
			Triangle[1][1][0][1] -= deltaS;

			Triangle[2][1][0][0] -= deltaS;
			Triangle[2][2][0][0] += deltaS;

			Triangle[3][1][0][0] += deltaS;
			Triangle[3][2][0][0] -= deltaS;

			if (Triangle[0][0][0][1] >= TRIWIDTH / 2) {
				isS = 1;
			}
		}
	}

	glutPostRedisplay();
	glutTimerFunc(10, Timer, 1);
}

GLvoid UpdateBuffer() {
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Triangle), Triangle, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
}

GLvoid Keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 't':
		if (!isT)
			isT = isTtmp;
		else {
			isTtmp = isT;
			isT = 0;
		}
		break;
	case 's':
		if (!isS)
			isS = isStmp;
		else {
			isStmp = isS;
			isS = 0;
		}
			
	}
}