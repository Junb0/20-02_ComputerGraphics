#include<iostream>
#include<random>

using namespace std;

class rectangle {
	int x1, y1;		// 좌측 하단
	int x2, y2;		// 우측 상단
public:
	rectangle() {
		random_device rn;
		default_random_engine dre(rn());
		int temp1, temp2;
		temp1 = dre() % 501;
		while (1) {
			temp2 = dre() % 501;
			if (temp2 != temp1)
				break;
		}
		x1 = temp1 < temp2 ? temp1 : temp2;
		x2 = temp1 > temp2 ? temp1 : temp2;
		temp1 = dre() % 501;
		while (1) {
			temp2 = dre() % 501;
			if (temp2 != temp1)
				break;
		}
		y1 = temp1 < temp2 ? temp1 : temp2;
		y2 = temp1 > temp2 ? temp1 : temp2;
	}
	int Left() {
		return x1;
	}
	int Right() {
		return x2;
	}
	int Bottom() {
		return y1;
	}
	int Top() {
		return y2;
	}
	bool isValidRange(int delta, bool isUp) {
		if (isUp == true) {
			if (y1 + delta < 0 || y2 + delta > 500)
				return false;
		}
		else {
			if (x1 + delta < 0 || x2 + delta > 500)
				return false;
		}
		return true;
	}
	bool move(int delta, char c) {
		switch (c) {
		case 'a':
			if (isValidRange(delta * -1, false) == true) {
				x1 -= delta;
				x2 -= delta;
			}
			else
				return false;
			break;
		case 'd':
			if (isValidRange(delta, false) == true) {
				x1 += delta;
				x2 += delta;
			}
			else
				return false;
			break;
		case 'w':
			if (isValidRange(delta, true) == true) {
				y1 += delta;
				y2 += delta;
			}
			else
				return false;
			break;
		case 's':
			if (isValidRange(delta * -1, true) == true) {
				y1 -= delta;
				y2 -= delta;
			}
			else
				return false;
			break;
		}
		return true;
	}
	void printCoord() {
		cout << "(" << x1 << ", " << y1 << ") (" << x2 << ", " << y2 << ")" << endl;
	}
};

bool isCollision(rectangle& r1, rectangle& r2);

int main() {
	rectangle r1, r2;
	char c;
	int delta = 30;
	cout << "(이동 Command: a(left) / d(right) / s(down) / w(up))" << endl;
	cout << "input Shape Coordinates value:" << endl;
	cout << "Shape 1: ";
	r1.printCoord();
	cout << "Shape 2: ";
	r2.printCoord();
	if (isCollision(r1, r2) == true)
		cout << "Rectangle 1 & Rectangle 2 collide!!" << endl;

	while (1) {
		cout << "Command: ";
		cin >> c;
		if ((c == 'w' || c == 'a' || c == 's' || c == 'd') == false) {
			cout << "invalid input!" << endl;
			continue;
		}
		if (r2.move(delta, c) == false) {
			cout << "invalid range!" << endl;
			continue;
		}
		cout << "Shape 1: ";
		r1.printCoord();
		cout << "Shape 2: ";
		r2.printCoord();
		if (isCollision(r1, r2) == true)
			cout << "Rectangle 1 & Rectangle 2 collide!!" << endl;
	}
}

bool isCollision(rectangle& const r1, rectangle& const r2) {
	if (r1.Right() >= r2.Left() && r1.Left() <= r2.Right() && r1.Bottom() <= r2.Top() && r1.Top() >= r2.Bottom())
		return true;
	return false;
}