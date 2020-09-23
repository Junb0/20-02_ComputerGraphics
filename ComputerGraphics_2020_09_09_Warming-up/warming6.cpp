#include<iostream>
using namespace std;

class rectangle {
public:
	int x1, y1;		// 좌측 상단
	int x2, y2;		// 우측 하단
};

void drawRect(rectangle rect);

int main() {
	rectangle rect;
	char c;
	cout << "input coord value (1~30) : ";
	cin >> rect.x1 >> rect.y1 >> rect.x2 >> rect.y2;
	rect.x1--;
	rect.x2--;
	rect.y1--;
	rect.y2--;

	drawRect(rect);

	while (1) {
		cin >> c;
		switch (c) {
		case 'x':
			if (rect.x2 < 29) {
				rect.x1++;
				rect.x2++;
			}
			break;
		case 'X':
			if (rect.x1 > 0) {
				rect.x1--;
				rect.x2--;
			}
			break;
		case 'y':
			if (rect.y2 < 29) {
				rect.y1++;
				rect.y2++;
			}
			break;
		case 'Y':
			if (rect.y1 > 0) {
				rect.y1--;
				rect.y2--;
			}
			break;
		case 's':
			if (rect.x2 > rect.x1&& rect.y2 > rect.y1) {
				rect.x2--;
				rect.y2--;
			}
			break;
		case 'S':
			if (rect.x2 < 29 && rect.y2 < 29) {
				rect.x2++;
				rect.y2++;
			}
			else if (rect.x1 > 0 && rect.y1 > 0) {
				rect.x1--;
				rect.y1--;
			}
			break;
		case 'i':
			if (rect.x2 < 29) {
				rect.x2++;
			}
			else if (rect.x1 > 0) {
				rect.x1--;
			}
			break;
		case 'j':
			if (rect.x2 > rect.x1) {
				rect.x2--;
			}
			break;
		case 'k':
			if (rect.y2 < 29) {
				rect.y2++;
			}
			else if (rect.y1 > 0) {
				rect.y1--;
			}
			break;
		case 'l':
			if (rect.y2 > rect.y1) {
				rect.y2--;
			}
			break;
		case 'r':
			cout << "input coord value (1~30) : ";
			cin >> rect.x1 >> rect.y1 >> rect.x2 >> rect.y2;
			rect.x1--;
			rect.x2--;
			rect.y1--;
			rect.y2--;
			break;
		case 'q':
			return 0;
		default:
			cout << "잘못된 입력입니다." << endl;
			break;
		}
		drawRect(rect);
	}
}

void drawRect(rectangle rect) {
	for (int i = 0; i < 30; ++i) {
		for (int j = 0; j < 30; ++j) {
			if (i >= rect.y1 && i <= rect.y2 && j >= rect.x1 && j <= rect.x2) {
				cout << '0';
			}
			else
				cout << '.';
			cout << ' ';
		}
		cout << endl;
	}
}