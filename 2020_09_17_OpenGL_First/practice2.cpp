#include<iostream>
#include<random>
#include<gl/glew.h>
#include<gl/freeglut.h>
#include<gl/freeglut_ext.h>
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Timer(int value);
GLvoid Mouse(int button, int state, int x, int y);

class color {
public:
	float r, g, b;
};

color Color_BG{ 1.0f, 1.0f, 0.0f };
color Color_Rect{ 0.0f, 1.0f, 0.0f };

std::random_device rd;
std::default_random_engine dre(rd());
std::uniform_real_distribution<float> urd(0.0f, 1.0f);

int main(int argc, char** argv) {						//---������ ����ϰ� �ݹ��Լ� ����
	//--- ������ ����
	glutInit(&argc, argv);								// glut �ʱ�ȭ
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);		// ���÷��� ��� ����
	glutInitWindowPosition(0, 0);						// �������� ��ġ ����
	glutInitWindowSize(400, 400);						// �������� ũ�� ����
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
	glutMainLoop();										// �̺�Ʈ ó�� ����
}

GLvoid drawScene() {									//---�ݹ��Լ� : �׸��� �ݹ� �Լ�
	glClearColor(Color_BG.r, Color_BG.g, Color_BG.b, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);						// ������ ������ ��ü�� ĥ�ϱ�
	// �׸��� �κ� ����
	glColor3f(Color_Rect.r, Color_Rect.g, Color_Rect.b);
	glRectf(-0.5f, -0.5f, 0.5f, 0.5f);
	//---�׸��� ���� �κ��� ���⿡ ���Ե�
	glutSwapBuffers();									// ȭ�鿡 ����ϱ�
}

GLvoid Reshape(int w, int h) {							//---�ݹ��Լ� : �ٽ� �׸��� �ݹ� �Լ�
	glViewport(0, 0, w, h);
}

void Mouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		if (x >= 100 && x <= 300 && y >= 100 && y <= 300)
			Color_Rect = { urd(dre), urd(dre), urd(dre) };
		else
			Color_BG = { urd(dre), urd(dre), urd(dre) };
	}
}