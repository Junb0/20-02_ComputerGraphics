#include<iostream>
#include<random>
#include<gl/glew.h>
#include<gl/freeglut.h>
#include<gl/freeglut_ext.h>
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid Timer(int value);

class color {
public:
	float r, g, b;
};

color cl{ 1.0f, 1.0f, 1.0f };
bool isAuto = false;
std::random_device rd;
std::default_random_engine dre(rd());
std::uniform_real_distribution<float> urd(0.0f, 1.0f);

int main(int argc, char** argv) {						//---������ ����ϰ� �ݹ��Լ� ����
	//--- ������ ����
	glutInit(&argc, argv);								// glut �ʱ�ȭ
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);		// ���÷��� ��� ����
	glutInitWindowPosition(0, 0);						// �������� ��ġ ����
	glutInitWindowSize(800, 600);						// �������� ũ�� ����
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
	glutKeyboardFunc(Keyboard);							// Ű���� �Է� �ݹ��Լ� ����
	glutMainLoop();										// �̺�Ʈ ó�� ����
}

GLvoid drawScene() {									//---�ݹ��Լ� : �׸��� �ݹ� �Լ�
	glClearColor(cl.r, cl.g, cl.b, 1.0f);				
	glClear(GL_COLOR_BUFFER_BIT);						// ������ ������ ��ü�� ĥ�ϱ�
	// �׸��� �κ� ����
	//---�׸��� ���� �κ��� ���⿡ ���Ե�
	glutSwapBuffers();									// ȭ�鿡 ����ϱ�
}

GLvoid Reshape(int w, int h) {							//---�ݹ��Լ� : �ٽ� �׸��� �ݹ� �Լ�
	glViewport(0, 0, w, h);
}

GLvoid Keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'R':
		cl = { 1.0f, 0.0f, 0.0f };
		break;
	case 'G':
		cl = { 0.0f, 1.0f, 0.0f };
		break;
	case 'B':
		cl = { 0.0f, 0.0f, 1.0f };
		break;
	case 'A':
		cl = { urd(dre), urd(dre), urd(dre) };
		break;
	case 'W':
		cl = { 1.0f, 1.0f, 1.0f };
		break;
	case 'K':
		cl = { 0.0f, 0.0f, 0.0f };
		break;
	case 'T':
		if (isAuto == false) {
			isAuto = true;
			glutTimerFunc(100, Timer, 1);
		}
		break;
	case 'S':
		isAuto = false;
		break;
	case 'Q':
		glutLeaveMainLoop();
		break;
	}
	glutPostRedisplay();
}

GLvoid Timer(int value) {
	cl = { urd(dre), urd(dre), urd(dre) };
	glutPostRedisplay();
	if(isAuto)glutTimerFunc(100, Timer, 1);
}