#include<iostream>
#include<random>
#include<list>
#include<gl/glew.h>
#include<gl/freeglut.h>
#include<gl/freeglut_ext.h>

#define LEFT 1
#define RIGHT 2

#define XX(x) (float)(x - (width / 2)) / (float)width * 2.0f
#define YY(y) (-1.0f) * (float)(y - (height / 2)) / (float)height * 2.0f

#define SPAWNCOOLTIME 2.0f

using namespace std;

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Timer(int value);
GLvoid Mouse(int button, int state, int x, int y);
GLvoid Motion(int x, int y);
GLvoid Keyboard(unsigned char key, int x, int y);

std::random_device rd;
std::default_random_engine dre(rd());
std::uniform_real_distribution<float> urd(0.0f, 1.0f);
std::uniform_real_distribution<float> urd2(-1.0f, 1.0f);
std::uniform_real_distribution<float> urdDeltaSpeed(0.3f, 1.0f);
std::uniform_real_distribution<float> urdDeltaSpeedX(0.6f, 1.0f);
std::uniform_real_distribution<float> urdDeltaSpeedY(-0.2f, 0.2f);

std::uniform_real_distribution<float> urdMakeY(0.0f, 0.6f);

class color {
public:
	float r = urd(dre), g = urd(dre), b = urd(dre);
	float a = 1.0f;
};

class coord {
public:
	float x = 0.0f, y = 0.0f;
};

class Object {
public:
	color cl;
	std::vector<coord> vertexs;
	coord startPoint;
	coord endPoint;
	bool isFalling = false;
	float dx = urdDeltaSpeedX(dre);
	float dy = urdDeltaSpeedY(dre);
	float gravity = urdDeltaSpeed(dre) * -1.0;

	coord getCenter() {
		coord tmp;
		float tmpX = 0.0f;
		float tmpY = 0.0f;
		if (this->vertexs.size() >= 3) {
			for (std::vector<coord>::iterator iter = vertexs.begin(); iter != vertexs.end(); iter++) {
				tmpX += (*iter).x;
				tmpY += (*iter).y;
			}
			tmp.x = tmpX / vertexs.size();
			tmp.y = tmpY / vertexs.size();
		}
		else {
			cout << "error : 도형의 무게중심을 구할 수 없음." << endl;
		}
		return tmp;
	}
};

std::list<Object> objs;
std::list<Object> grid_objs;

bool grid[3][8] = { {0,}, };

int width = 600, height = 600;
int nRect = 0;
bool isDrag = false;
bool isScaleMove = false;
bool isColorMove = false;

bool isFill = true;
bool isDrawPath = false;

float speed = 1.0f;

coord startPoint;	// 마우스 드래그 시작점
coord endPoint;		// 마우스 드래그 끝점

int old_t;	// 델타타임 구하는 용도
float spawnCnt = 0.0f;	// spawnCnt가 SPAWNCOOLTIME에 도달할때 마다 도형 랜덤 생성

bool GetIntersectPoint(const coord& AP1, const coord& AP2,			// 선분과 선분의 교차점 검사 및 IP에 교차점 저장
	const coord& BP1, const coord& BP2, coord* IP)
{
	float t;
	float s;
	float under = (BP2.y - BP1.y) * (AP2.x - AP1.x) - (BP2.x - BP1.x) * (AP2.y - AP1.y);
	if (under == 0) return false;

	float _t = (BP2.x - BP1.x) * (AP1.y - BP1.y) - (BP2.y - BP1.y) * (AP1.x - BP1.x);
	float _s = (AP2.x - AP1.x) * (AP1.y - BP1.y) - (AP2.y - AP1.y) * (AP1.x - BP1.x);

	t = _t / under;
	s = _s / under;

	if (t < 0.0 || t>1.0 || s < 0.0 || s>1.0) return false;
	if (_t == 0 && _s == 0) return false;

	IP->x = AP1.x + t * (float)(AP2.x - AP1.x);
	IP->y = AP1.y + t * (float)(AP2.y - AP1.y);

	return true;
}

Object MakeTriangle() {
	Object Obj;
	switch (dre() % 3) {
	case 0:
		Obj.vertexs.push_back(coord{ 0, 0.25 });
		Obj.vertexs.push_back(coord{ 0.25, -0.25 });
		Obj.vertexs.push_back(coord{ -0.25, -0.25 });
		break;
	case 1:
		Obj.vertexs.push_back(coord{ 0, 0.25 });
		Obj.vertexs.push_back(coord{ 0.25, 0.0 });
		Obj.vertexs.push_back(coord{ -0.25, -0.25 });
		break;
	case 2:
		Obj.vertexs.push_back(coord{ 0.25, 0.25 });
		Obj.vertexs.push_back(coord{ 0.2, -0.1 });
		Obj.vertexs.push_back(coord{ -0.25, -0.2 });
		break;
	}
	float tmpY = urdMakeY(dre);
	if (dre() % 2) {		// 오른쪽에서 생성되어 왼쪽으로 날아감
		for (std::vector<coord>::iterator iterV = Obj.vertexs.begin(); iterV != Obj.vertexs.end(); ++iterV) {
			(*iterV).x += 1.25;
			(*iterV).y += tmpY;
		}
		Obj.dx *= -1;
	}
	else {					// 왼쪽에서 생성되어 오른쪽으로 날아감
		for (std::vector<coord>::iterator iterV = Obj.vertexs.begin(); iterV != Obj.vertexs.end(); ++iterV) {
			(*iterV).x -= 1.25;
			(*iterV).y += tmpY;
		}
	}
	Obj.startPoint = Obj.getCenter();
	if (Obj.dx > 0.0f) {	// 도형이 오른쪽으로 날아갈때
		Obj.endPoint.x = 1.0f;
		Obj.endPoint.y = Obj.startPoint.y + ((1.0f - Obj.startPoint.x) / Obj.dx * Obj.dy);
	}
	else {
		Obj.endPoint.x = -1.0f;
		Obj.endPoint.y = Obj.startPoint.y + ((-1.0f - Obj.startPoint.x) / Obj.dx * Obj.dy);
	}
	cout << endl << "삼각형 생성" << endl;
	return Obj;
}

int main(int argc, char** argv) {						//---윈도우 출력하고 콜백함수 설정
	//--- 윈도우 생성
	glutInit(&argc, argv);								// glut 초기화
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);		// 디스플레이 모드 설정
	glutInitWindowPosition(0, 0);						// 윈도우의 위치 지정
	glutInitWindowSize(width, height);					// 윈도우의 크기 지정
	glutCreateWindow("Example1");						// 윈도우 생성(윈도우 이름)

	//--- GLEW 초기화하기
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {						// glew 초기화	
		std::cerr << "Unable to initialize GLEW" << std::endl;
		exit(EXIT_FAILURE);
	}
	else
		std::cout << "GLEW Initialized\n";

	objs.push_back(MakeTriangle());

	old_t = glutGet(GLUT_ELAPSED_TIME);

	glutTimerFunc(10, Timer, 1);
	glutDisplayFunc(drawScene);							// 출력 콜백함수의 지정
	glutReshapeFunc(Reshape);							// 다시 그리기 콜백함수 지정
	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);
	glutKeyboardFunc(Keyboard);
	glutMainLoop();										// 이벤트 처리 시작
}

GLvoid drawScene() {									//---콜백함수 : 그리기 콜백 함수
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);						// 설정된 색으로 전체를 칠하기

	for (std::list<Object>::iterator iterO = objs.begin(); iterO != objs.end(); iterO++) {
		glColor3f((*iterO).cl.r, (*iterO).cl.g, (*iterO).cl.b);
		if (isFill) {	// 도형 모드 FILL
			for (int i = 0; i + 2 < (*iterO).vertexs.size(); ++i) {
				glBegin(GL_TRIANGLES);
				glVertex2f((*iterO).vertexs[0].x, (*iterO).vertexs[0].y);
				glVertex2f((*iterO).vertexs[i + 1].x, (*iterO).vertexs[i + 1].y);
				glVertex2f((*iterO).vertexs[i + 2].x, (*iterO).vertexs[i + 2].y);
				glEnd();
			}
		}
		else {		// 도형 모드 LINE
			for (int i = 0; i + 2 < (*iterO).vertexs.size(); ++i) {
				glBegin(GL_LINE_LOOP);
				glVertex2f((*iterO).vertexs[0].x, (*iterO).vertexs[0].y);
				glVertex2f((*iterO).vertexs[i + 1].x, (*iterO).vertexs[i + 1].y);

				glVertex2f((*iterO).vertexs[i + 1].x, (*iterO).vertexs[i + 1].y);
				glVertex2f((*iterO).vertexs[i + 2].x, (*iterO).vertexs[i + 2].y);

				glVertex2f((*iterO).vertexs[i + 2].x, (*iterO).vertexs[i + 2].y);
				glVertex2f((*iterO).vertexs[0].x, (*iterO).vertexs[0].y);
				glEnd();
			}
		}
	}
	
	// 경로 그리기
	for (std::list<Object>::iterator iterO = objs.begin(); iterO != objs.end(); iterO++) {
		if (isDrawPath) {
			glColor3f(0.5f, 0.5f, 0.5f);
			glBegin(GL_LINE_LOOP);
			glVertex2f((*iterO).startPoint.x, (*iterO).startPoint.y);
			glVertex2f((*iterO).endPoint.x, (*iterO).endPoint.y);
			glEnd();
		}
	}

	// 격자 그리기
	for (int i = 0; i < 3; ++i) {
		glColor3f(0.5f, 0.5f, 1.0f);
		glBegin(GL_LINE_LOOP);
		glVertex2f(-1, (i + 1) * 0.15 - 1.0f);
		glVertex2f(1, (i + 1) * 0.15 - 1.0f);
		glEnd();
	}
	for (int i = 0; i < 7; ++i) {
		glColor3f(0.5f, 0.5f, 1.0f);
		glBegin(GL_LINE_LOOP);
		glVertex2f((i + 1) * 0.25 - 1.0f, -0.55f);
		glVertex2f((i + 1) * 0.25 - 1.0f, -1.0f);
		glEnd();
	}

	// 마우스 드래그 시 절단선 그리기
	if (isDrag) {
		glColor3f(0.5f, 0.5f, 0.5f);
		glBegin(GL_LINE_LOOP);
		glVertex2f(startPoint.x, startPoint.y);
		glVertex2f(endPoint.x, endPoint.y);
		glEnd();
	}

	glutSwapBuffers();									// 화면에 출력하기
}

GLvoid Reshape(int w, int h) {							//---콜백함수 : 다시 그리기 콜백 함수
	glViewport(0, 0, w, h);
}

GLvoid Mouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		isDrag = true;
		startPoint.x = XX(x);
		startPoint.y = YY(y);

		endPoint.x = XX(x);
		endPoint.y = YY(y);

		cout << endl << "드래그 시작 x : "<< startPoint.x << " y : " << startPoint.y << endl;
	}
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		isDrag = false;
		endPoint.x = XX(x);
		endPoint.y = YY(y);
		cout << endl << "드래그 완료 x : " << endPoint.x << " y : " << endPoint.y << endl;

		// 도형 분할
		std::vector<Object> objsVector;	// objs 루프 종료 후, objs에 추가할 오브젝트들 임시 저장 벡터
		for (std::list<Object>::iterator iterO = objs.begin(); iterO != objs.end();) {
			vector<coord> tmpVector; // 검사된 교차점이 저장되는 벡터
			// 교차점 검사
			int startIdx = -1;		// startIdx번째 버텍스 와 그 다음 버텍스 사이에 첫번째 교차점이 존재.
			int endIdx = -1;
			Object tmpObjA;
			Object tmpObjB;		// 분할된 두 도형. 우선 objsVector에 저장 후 루프 종료 시 objs에 추가
			for (int i = 0; i < (*iterO).vertexs.size(); ++i) {
				coord tmp;
				if (i == (*iterO).vertexs.size() - 1) {		// 마지막 점과 첫번째 점과의 검사
					if (GetIntersectPoint((*iterO).vertexs[i], (*iterO).vertexs[0], startPoint, endPoint, &tmp)) {
						tmpVector.push_back(tmp);
						if (startIdx == -1)		// (사실 이 경우는 불가능함..)
							startIdx = i;
						else
							endIdx = i;
						cout << endl << "교차점 확인" << endl << endl;
						cout << i << "번째 버텍스(" << (*iterO).vertexs[i].x << "," << (*iterO).vertexs[i].y << ") 와 ";
						cout << 0 << "번째 버텍스(" << (*iterO).vertexs[0].x << "," << (*iterO).vertexs[0].y << ") 사이의" << endl;
						cout << "점(" << tmp.x << "," << tmp.y << ")" << endl << endl;
					}
				}
				else {
					if (GetIntersectPoint((*iterO).vertexs[i], (*iterO).vertexs[i+1], startPoint, endPoint, &tmp)) {
						tmpVector.push_back(tmp);
						if (startIdx == -1)
							startIdx = i;
						else
							endIdx = i;
						cout << "교차점 확인" << endl;
						cout << i << "번째 버텍스(" << (*iterO).vertexs[i].x << "," << (*iterO).vertexs[i].y << ") 와 ";
						cout << i + 1 << "번째 버텍스(" << (*iterO).vertexs[i + 1].x << "," << (*iterO).vertexs[i + 1].y << ") 사이의" << endl;
						cout << "점(" << tmp.x << "," << tmp.y << ")" << endl << endl;
					}
				}
			}

			if (tmpVector.size() == 2) {		// 두개의 교차점 발견시 도형 분할 실시
				cout << "도형 분할합니다." << endl;
				// Object A 생성
				for (int i = 0; i <= startIdx; ++i) {
					tmpObjA.vertexs.push_back((*iterO).vertexs[i]);
				}
				tmpObjA.vertexs.push_back(tmpVector[0]);
				tmpObjA.vertexs.push_back(tmpVector[1]);
				for (int i = endIdx + 1; i < (*iterO).vertexs.size(); ++i) {
					tmpObjA.vertexs.push_back((*iterO).vertexs[i]);
				}
				cout << "오브젝트 A : ";
				for (coord c : tmpObjA.vertexs) {
					cout << "(" << c.x << "," << c.y << ") ";
				}

				// Object B 생성
				tmpObjB.vertexs.push_back(tmpVector[0]);
				for (int i = startIdx + 1; i <= endIdx; ++i) {
					tmpObjB.vertexs.push_back((*iterO).vertexs[i]);
				}
				tmpObjB.vertexs.push_back(tmpVector[1]);
				cout << endl << "오브젝트 B : ";
				for (coord c : tmpObjB.vertexs) {
					cout << "(" << c.x << "," << c.y << ") ";
				}

				// 생성된 두 도형이 떨어지도록 설정
				tmpObjA.isFalling = true;
				tmpObjB.isFalling = true;

				// objs에 Object A, B push
				objsVector.push_back(tmpObjA);
				objsVector.push_back(tmpObjB);
				iterO = objs.erase(iterO);
			}
			else {
				iterO++;
			}
		}
		for (Object O : objsVector) {
			objs.push_back(O);
		}
	}
	glutPostRedisplay();
}

GLvoid Keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'm':
		if (isFill)
			isFill = false;
		else
			isFill = true;
		break;
	case 'p':
		if (isDrawPath)
			isDrawPath = false;
		else
			isDrawPath = true;
		break;
	case '+':
		speed += 0.1f;
		break;
	case '-':
		if (speed >= 0.2f)
			speed -= 0.1f;
		break;
	case 'q':
	case 'Q':
		glutLeaveMainLoop();
		break;
	}
}


GLvoid Timer(int value) {
	int t;
	float dt;

	t = glutGet(GLUT_ELAPSED_TIME);
	dt = (t - old_t) / 1000.0;
	old_t = t;

	if (spawnCnt >= SPAWNCOOLTIME) {
		objs.push_back(MakeTriangle());
		spawnCnt = 0.0f;
	}
	spawnCnt += dt;

	// isFalling == false 인 오브젝트 날아가게 하기
	for (std::list<Object>::iterator iterO = objs.begin(); iterO != objs.end();) {
		if ((*iterO).isFalling == false) {
			for (std::vector<coord>::iterator iterV = (*iterO).vertexs.begin(); iterV != (*iterO).vertexs.end(); ++iterV) {
				(*iterV).x += (*iterO).dx * dt * speed;
				(*iterV).y += (*iterO).dy * dt * speed;
			}
			// 범위 밖 도형 삭제
			if ((*iterO).getCenter().x > 1.5f || (*iterO).getCenter().x < -1.5f) {
				cout << endl << "도형 범위밖으로 벗어나 삭제" << endl;
				iterO = objs.erase(iterO);
			}
			else
				iterO++;
		}
		else
			iterO++;
	}

	// isFalling == true 인 오브젝트 떨어지게 하기
	for (std::list<Object>::iterator iterO = objs.begin(); iterO != objs.end(); ++iterO) {
		if ((*iterO).isFalling == true) {
			for (std::vector<coord>::iterator iterV = (*iterO).vertexs.begin(); iterV != (*iterO).vertexs.end(); ++iterV)
				(*iterV).y += (*iterO).gravity * dt;
		}
	}

	glutPostRedisplay();
	glutTimerFunc(10, Timer, 1);
}

GLvoid Motion(int x, int y) {
	if (isDrag == true) {
		endPoint.x = XX(x);
		endPoint.y = YY(y);
		//cout << "마우스 이동 x : " << endPoint.x << " y : " << endPoint.y << endl;
	}
}
