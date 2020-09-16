#include<iostream>
#include<random>
#include<gl/glew.h>
#include<gl/freeglut.h>
#include<gl/freeglut_ext.h>
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Timer(int value);
GLvoid Mouse(int button, int state, int x, int y);
GLvoid Keyboard(unsigned char key, int x, int y);

class color {
public:
	float r, g, b;
};

class coord {
public:
	float x, y;
};

std::random_device rd;
std::default_random_engine dre(rd());
std::uniform_real_distribution<float> urd(0.0f, 1.0f);

color Color_BG{ 0.2f, 0.2f, 0.2f };
color Color_Rect{ urd(dre), urd(dre) , urd(dre) };
coord pivot{ 0.0f, 0.0f };

int width = 400, height = 400;
float dx = 0.013f, dy = 0.01f;
bool isMove = false;

int main(int argc, char** argv) {						//---������ ����ϰ� �ݹ��Լ� ����
	//--- ������ ����
	glutInit(&argc, argv);								// glut �ʱ�ȭ
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);		// ���÷��� ��� ����
	glutInitWindowPosition(0, 0);						// �������� ��ġ ����
	glutInitWindowSize(width, height);					// �������� ũ�� ����
	glutCreateWindow("Example1");						// ������ ����(������ �̸�)

	//--- GLEW �ʱ�ȭ�ϱ�
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {						// glew �ʱ�ȭ	
		std::cerr << "Unable to initialize GLEW" << std::endl;
		exit(EXIT_FAILURE);
	}
	else
		std::cout << "GLEW Initialized\n";

	glutDisplayFunc(drawScene);							// ��� �ݹ��Լ��� ����
	glutReshapeFunc(Reshape);							// �ٽ� �׸��� �ݹ��Լ� ����
	glutMouseFunc(Mouse);
	glutKeyboardFunc(Keyboard);
	glutMainLoop();										// �̺�Ʈ ó�� ����
}

GLvoid drawScene() {									//---�ݹ��Լ� : �׸��� �ݹ� �Լ�
	glClearColor(Color_BG.r, Color_BG.g, Color_BG.b, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);						// ������ ������ ��ü�� ĥ�ϱ�
	// �׸��� �κ� ����
	glColor3f(Color_Rect.r, Color_Rect.g, Color_Rect.b);
	glRectf(pivot.x - 0.1f, pivot.y - 0.1f, pivot.x + 0.1f, pivot.y + 0.1f);
	//---�׸��� ���� �κ��� ���⿡ ���Ե�
	glutSwapBuffers();									// ȭ�鿡 ����ϱ�
}

GLvoid Reshape(int w, int h) {							//---�ݹ��Լ� : �ٽ� �׸��� �ݹ� �Լ�
	glViewport(0, 0, w, h);
}

GLvoid Mouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		pivot.x = (float)(x - (width / 2)) / (float)width * 2.0f;
		pivot.y = -1.0f * (float)(y - (width / 2)) / (float)width * 2.0f;
		Color_Rect = { urd(dre), urd(dre) , urd(dre) };
	}
	glutPostRedisplay();
}

GLvoid Keyboard(unsigned char key, int x, int y) {
	switch (key) {;
	case 'a':
		if (isMove == false) {
			isMove = true;
			glutTimerFunc(10, Timer, 1);
		}
		break;
	case 's':
		isMove = false;
		break;
	}
	glutPostRedisplay();
}

GLvoid Timer(int value) {
	if ((pivot.x + 0.1f >= 1.0f && dx >= 0.0f) || (pivot.x - 0.1f <= -1.0f && dx <= 0.0f))
		dx *= -1.0f;
	if ((pivot.y + 0.1f >= 1.0f && dy >= 0.0f)|| (pivot.y - 0.1f <= -1.0f && dy <= 0.0f))
		dy *= -1.0f;
	pivot.x += dx;
	pivot.y += dy;
	glutPostRedisplay();
	if (isMove)glutTimerFunc(10, Timer, 1);
}