#include<stdlib.h>
#include<stdio.h>
#include<iostream>
#include<random>
#include<list>
#include<gl/glew.h>
#include<gl/freeglut.h>
#include<gl/freeglut_ext.h>
GLvoid drawScene();
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid Mouse(int button, int state, int x, int y);

GLvoid Timer(int value);
GLvoid InitBuffer();
GLvoid InitShader();

GLuint VAO, VBO[2];

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
	// ���ؽ� ���̴� �о� �����ϰ� ������ �ϱ�
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
		cerr << "ERROR:vertex shader ������ ����\n" << errorLog << endl;
	}
}


void make_fragmentShaders() {
	GLchar* fragmentsource;
	fragmentsource = filetobuf("fragment1.glsl");
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
	width = 500;
	height = 500;

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(width, height);
	glutCreateWindow("Example1");

	glewExperimental = GL_TRUE;
	glewInit();

	InitShader();
	InitBuffer();
	

	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutMouseFunc(Mouse);

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

GLvoid Reshape(int w, int h) {							//---�ݹ��Լ� : �ٽ� �׸��� �ݹ� �Լ�
	glViewport(0, 0, w, h);
}

GLvoid InitBuffer() {
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(2, VBO);
	//1��° VBOȰ��ȭ, ���ε�, ��ǥ�� ����
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, 36 * sizeof(GLfloat), triShape, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, 36 * sizeof(GLfloat), colors, GL_STATIC_DRAW);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(1);
	
}

GLvoid Mouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		for (int i = 0; i < 3; ++i) {
			triShape[cnt][i][0] = ref_triShape[i][0] + (float)(x - (width / 2)) / (float)width * 2.0f;
			cout << "x : " << triShape[cnt][i][0] << endl;
		}
		for (int i = 0; i < 3; ++i) {
			triShape[cnt][i][1] = ref_triShape[i][1] + (-1.0f) * (float)(y - (width / 2)) / (float)width * 2.0f;
			cout << "y : "<< triShape[cnt][i][1] << endl;
		}
		cnt++;
		cnt %= 4;
	}
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, 36 * sizeof(GLfloat), triShape, GL_STATIC_DRAW);
	glutPostRedisplay();
}