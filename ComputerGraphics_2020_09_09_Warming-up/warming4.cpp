#include<iostream>
#include<vector>
#include<algorithm>
// 피타고라스 법칙으로 거리구하기 매크로 함수
#define calcLen(a,b) sqrt( (a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y) + (a.z - b.z) *  (a.z - b.z) )
#define _back(a) a > 0 ? a - 1 : 9
#define _front(a) a < 9 ? a + 1 : 0
using namespace std;

typedef struct _Point {
	// 점의 x,y,z좌표는 실수로 double 타입을 사용함.
	double x, y, z;
	bool isEx = false;
}point;

const point Origin = { 0,0,0 };

void printList(vector<point>& const list);

int main() {
	int front = 0, back = 0, cnt = 0;
	int nTmp;
	vector<point> list(10);
	double tmp1, tmp2, tmp3;
	double distTmp;
	point tmp;
	char c;
	while (1) {
		printList(list);
		cin >> c;
		switch (c)
		{
		case '+':
			cin >> tmp1 >> tmp2 >> tmp3;
			if (cnt >= 10) {
				cout << "list is full." << endl;
				continue;
			}
			list[front] = { tmp1, tmp2, tmp3, true};
			front = _front(front);
			cnt++;
			break;

		case '-':
			if (cnt <= 0) {
				cout << "list is empty," << endl;
				continue;
			}
			front = _back(front);
			list[front].isEx = false;
			cnt--;
			break;

		case 'e':
			cin >> tmp1 >> tmp2 >> tmp3;
			if (cnt >= 10) {
				cout << "list is full." << endl;
				continue;
			}
			tmp = list[9];
			for (int i = 8; i >= 0; --i)
				list[i + 1] = list[i];
			list[0] = tmp;
			list[back] = { tmp1, tmp2, tmp3, true };
			front = _front(front);
			cnt++;
			break;

		case 'd':
			if (cnt <= 0) {
				cout << "list is empty," << endl;
				continue;
			}
			list[back].isEx = false;
			back = _front(back);
			cnt--;
			break;

		case 'l':
			cout << "리스트의 길이 : " << cnt << endl << endl;
			break;

		case 'c':
			cout << "리스트를 초기화 합니다." << endl << endl;
			front = 0, back = 0, cnt = 0;
			for (int i = 0; i < 10; ++i)
				list[i].isEx = false;
			break;

		case 'm':
			if (cnt <= 0) {
				cout << "list is empty." << endl;
				continue;
			}
			distTmp = calcLen(Origin, list[back]);
			nTmp = back;
			for (int i = 0; i < 10; ++i) {
				if (list[i].isEx == true) {
					if (calcLen(Origin, list[i]) > distTmp) {
						distTmp = calcLen(Origin, list[i]);
						nTmp = i;
					}
				}
			}
			cout << "원점에서 가장 멀리 있는 점 : " << list[nTmp].x << ' ' << list[nTmp].y << ' ' << list[nTmp].z << "  length : " << calcLen(Origin, list[nTmp]) << endl << endl;
			break;

		case 'n':
			if (cnt <= 0) {
				cout << "list is empty." << endl;
				continue;
			}
			distTmp = calcLen(Origin, list[back]);
			nTmp = back;
			for (int i = 0; i < 10; ++i) {
				if (list[i].isEx == true) {
					if (calcLen(Origin, list[i]) < distTmp) {
						distTmp = calcLen(Origin, list[i]);
						nTmp = i;
					}
				}
			}
			cout << "원점에서 가장 가까이 있는 점 : " << list[nTmp].x << ' ' << list[nTmp].y << ' ' << list[nTmp].z << "  length : " << calcLen(Origin, list[nTmp]) << endl << endl;
			break;

		case 's':
			sort(list.begin(), list.end(), [](point a, point b)
				{
					if (a.isEx == true && b.isEx == true) {
						return calcLen(Origin, a) > calcLen(Origin, b);
					}
					if (a.isEx == true && b.isEx == false) {
						return true;
					}
					return false;
				});
			back = 0;
			front = cnt;
			if (front >= 10)
				front = 0;
			break;

		case 'q':
			return 0;
			break;
		default:
			cout << "잘못된 입력입니다.";
		}
	}
}

void printList(vector<point>& const list) {
	for (int i = 9; i >= 0; --i) {
		cout << i << " : ";
		if (list[i].isEx == true)
			cout << list[i].x << ' ' << list[i].y << ' ' << list[i].z << "  length : " << calcLen(Origin, list[i]);
		cout << endl;
	}
	cout << endl;
}