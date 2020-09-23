#include<iostream>
#include<random>
#include<vector>
#include<algorithm>
using namespace std;

class Point {
public:
	int x = 0, y = 0;
};

void drawMap(bool arr[50][50]);

int main() {
	int cnt = 0, tmp;
	int upI, upN;
	int leftI, leftN;
	char c;
	Point p;
	random_device rd;
	default_random_engine dre(rd());

	

	bool arr[50][50];
	vector<int> vQ;
	vector<int> hQ;
	while (1) {
		while (1) {
			if (cnt == 49)
				break;
			if (cnt < 49 && cnt >= 45) {
				vQ.push_back(49 - cnt);
				hQ.push_back(49 - cnt);
				break;
			}
			tmp = dre() % 4 + 1;
			vQ.push_back(tmp);
			hQ.push_back(tmp);
			cnt += tmp;
		}

		upI = dre() % 10 + 10;
		upN = dre() % 4 + 1;
		leftI = dre() % 10 + 30;
		leftN = dre() % 4 + 1;

		vQ.push_back(upN);
		hQ.push_back(leftN);

		shuffle(vQ.begin(), vQ.end(), dre);
		shuffle(hQ.begin(), hQ.end(), dre);

		for (int i = 0; i < 50; ++i)
			for (int j = 0; j < 50; ++j)
				arr[i][j] = 1;
		arr[0][0] = 0;
		arr[49][49] = 0;
		while (!(hQ.empty() && vQ.empty())) {
			if (!hQ.empty()) {
				for (int i = 0; i < hQ.back(); ++i) {
					arr[p.y][p.x] = 0;
					p.x++;
				}
				hQ.pop_back();
			}
			if (p.y >= upI) {
				upI = 10000;
				for (int i = 0; i < upN; ++i) {
					arr[p.y][p.x] = 0;
					p.y--;
				}
				continue;
			}
			if (!vQ.empty()) {
				for (int i = 0; i < vQ.back(); ++i) {
					arr[p.y][p.x] = 0;
					p.y++;
				}
			}
			vQ.pop_back();
			if (p.x >= leftI) {
				leftI = 10000;
				for (int i = 0; i < leftN; ++i) {
					arr[p.y][p.x] = 0;
					p.x--;
				}
				if (!vQ.empty()) {
					for (int i = 0; i < vQ.back(); ++i) {
						arr[p.y][p.x] = 0;
						p.y++;
					}
				}
				continue;
			}
		}

		drawMap(arr);

		std::cin >> c;

		switch (c) {
		case 'r':
			cnt = 0;
			vQ.clear();
			hQ.clear();
			p.x = 0;
			p.y = 0;
			break;
		case 'q':
			return 0;
		default:
			cout << "잘못된 입력입니다." << endl << endl;
			return 0;
		}
	}
}

void drawMap(bool arr[50][50]) {
	for (int i = 0; i < 50; ++i) {
		for (int j = 0; j < 50; ++j) {
			cout << arr[i][j] << ' ';
		}
		cout << endl;
	}
	cout << endl;
}