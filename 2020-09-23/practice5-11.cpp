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

#define XX(x) (float)(x - (width / 2)) / (float)width * 2.0f
#define YY(y) (-1.0f) * (float)(y - (height / 2)) / (float)height * 2.0f

using namespace std;

GLvoid drawScene();
GLvoid Reshape(int w, int h);
GLvoid Timer(int value);
GLvoid InitBuffer();
GLvoid InitShader();
GLvoid UpdateBuffer();
GLvoid Mouse(int button, int state, int x, int y);
GLvoid Motion(int x, int y);

GLuint VAO[2], VBO[2];

random_device rd;
default_random_engine dre(rd());
uniform_real_distribution<> urd1(-0.7f, 0.7f);
uniform_real_distribution<> urd2(0.005f, 0.02f);
uniform_real_distribution<> urdColor(0.0f, 1.0f);

int bindPoint = 0;	// 몇번 꼭지점을 클릭했는지
int bindType = 0;	// 점을 클릭했는지 선을 클릭했는지

float tmpX;
float tmpY;

struct CfPt {
	double x, y;
	double DistanceTo(CfPt P) {
		return sqrt((P.x - x) * (P.x - x) + (P.y - y) * (P.y - y));
	}
};

//선분과의 거리;
double SegmentDistance(CfPt A, CfPt B, CfPt P) {
	double lineLen = A.DistanceTo(B);
	if (lineLen == 0) return A.DistanceTo(P);//A == B;
	double prj = ((P.x - A.x) * (B.x - A.x) + (P.y - A.y) * (B.y - A.y)) / lineLen;
	if (prj < 0) return A.DistanceTo(P);
	else if (prj > lineLen) return B.DistanceTo(P);
	else {
		return fabs((-1) * (P.x - A.x) * (B.y - A.y) + (P.y - A.y) * (B.x - A.x)) / lineLen;
	}
}

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

GLfloat Rect[5][2][3] = {
	{{-0.5f, 0.5f, 0.0f}, {urdColor(dre), urdColor(dre), urdColor(dre)}},
	{{0.5f, 0.5f, 0.0f}, {urdColor(dre), urdColor(dre), urdColor(dre)}},
	{{0.5f, -0.5f, 0.0f}, {urdColor(dre), urdColor(dre), urdColor(dre)}},
	{{-0.5f, -0.5f, 0.0f}, {urdColor(dre), urdColor(dre), urdColor(dre)}},
	{{-0.5f, 0.5f, 0.0f}, {urdColor(dre), urdColor(dre), urdColor(dre)}}
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
	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);

	glutMainLoop();
}

GLvoid drawScene() {
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shaderID);

	glBindVertexArray(VAO[0]);	// 삼각형의 속성이 있는 VAO[0] 사용

	UpdateBuffer();
	glDrawArrays(GL_LINES, 0, 2);
	glDrawArrays(GL_LINES, 1, 2);
	glDrawArrays(GL_LINES, 2, 2);
	glDrawArrays(GL_LINES, 3, 2);

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

	glBufferData(GL_ARRAY_BUFFER, sizeof(Rect), Rect, GL_STATIC_DRAW);

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

GLvoid UpdateBuffer() {
	glBindVertexArray(VAO[0]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Rect), Rect, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
}

GLvoid Mouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		for (int i = 0; i < 4; ++i) {
			if (SegmentDistance(CfPt{ Rect[i][0][0], Rect[i][0][1] }, CfPt{ Rect[i + 1][0][0], Rect[i + 1][0][1] }, CfPt{ XX(x), YY(y) }) < 0.03f) {
				bindPoint = i + 1;
				bindType = 2;
				cout << SegmentDistance(CfPt{ Rect[i][0][0], Rect[i][0][1] }, CfPt{ Rect[i + 1][0][0], Rect[i + 1][0][1] }, CfPt{ XX(x), YY(y) }) << endl;
				cout << "bindPoint : " << bindPoint << ", ";
				cout << "bindType : " << bindType << endl;
				break;
			}
		}
		for (int i = 0; i < 4; ++i) {
			if (abs(Rect[i][0][0] - XX(x)) < 0.03f && abs(Rect[i][0][1] - YY(y)) < 0.03f) {
				bindPoint = i + 1;
				bindType = 1;
				tmpX = (Rect[bindPoint][0][0] - Rect[bindPoint - 1][0][0]);
				tmpY = (Rect[bindPoint][0][1] - Rect[bindPoint - 1][0][1]);
				cout << "bindPoint : " << bindPoint << ", ";
				cout << "bindType : " << bindType << endl;
				break;
			}
		}
	}
	else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		bindPoint = 0;
		bindType = 0;
		cout << "bindPoint free" << endl;
	}
}

GLvoid Motion(int x, int y) {
	if (bindType == 1) {
		if (bindPoint) {
			Rect[bindPoint - 1][0][0] = XX(x);
			Rect[bindPoint - 1][0][1] = YY(y);
			if (bindPoint - 1 == 0) {
				Rect[4][0][0] = XX(x);
				Rect[4][0][1] = YY(y);
			}
		}
	}
	/*else if (bindType == 2) {
		if (bindPoint) {
			Rect[bindPoint - 1][0][0] = XX(x) - tmpX / 2;
			Rect[bindPoint][0][0] = XX(x) + tmpX / 2;
		}
	}*/
	glutPostRedisplay();
}
