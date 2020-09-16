#include<iostream>
#include<random>

using namespace std;

void ResetArr(int arr1[3][3], int arr2[3][3]);
void PrintSumOfArr(int arr1[3][3], int arr2[3][3]);
void PrintDiffOfArr(int arr1[3][3], int arr2[3][3]);
void PrintMultiOfArr(int arr1[3][3], int arr2[3][3]);
void PrintDetOfArr(int arr1[3][3], int arr2[3][3]);
void PrintTransArr(int arr1[3][3], int arr2[3][3]);
void PrintHArr(int arr1[3][3], int arr2[3][3]);

int main() {
	int arr1[3][3] = { {0,0,1}, {1,1,1}, {0,1,0} };
	int arr2[3][3] = { {1,0,1}, {0,0,0}, {0,1,0} };
	char ch;
	//ResetArr(arr1, arr2);

	while (1) {
		cout << "현재 행렬" << endl;
		for (int i = 0; i < 3; ++i) {
			cout << arr1[i][0] << ' ' << arr1[i][1] << ' ' << arr1[i][2];
			cout << "   ";
			cout << arr2[i][0] << ' ' << arr2[i][1] << ' ' << arr2[i][2];
			cout << endl;
		}
		cout << endl << endl;
		cout << "명령어를 입력하세요." << endl;
		cin >> ch;

		if (cin.fail()) {
			cout << endl << "문자만 입력 가능합니다." << endl << endl;
			cin.clear();
			cin.ignore(CHAR_MAX, '\n');
			continue;
		}

		switch (ch) {
		case 'm':
			PrintMultiOfArr(arr1, arr2);
			cout << endl;
			break;
		case 'a':
			PrintSumOfArr(arr1, arr2);
			cout << endl;
			break;
		case 'd':
			PrintDiffOfArr(arr1, arr2);
			cout << endl;
			break;
		case 'r':
			PrintDetOfArr(arr1, arr2);
			cout << endl;
			break;
		case 't':
			PrintTransArr(arr1, arr2);
			cout << endl;
			break;
		case 'h':
			PrintHArr(arr1, arr2);
			cout << endl;
			break;
		case 's':
			cout << "행렬의 값을 랜덤하게 재설정합니다." << endl;
			ResetArr(arr1, arr2);
			cout << endl;
			break;
		case 'q':
			return 0;
		default:
			cout << "잘못된 입력입니다." << endl;
			break;
		}
	}
}

void ResetArr(int arr1[3][3], int arr2[3][3]) {
	random_device rn;
	default_random_engine dre(rn());
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			arr1[i][j] = dre() % 2;
			arr2[i][j] = dre() % 2;
		}
	}
}

void PrintSumOfArr(int arr1[3][3], int arr2[3][3]) {
	int arrsum[3][3];

	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < 3; ++j)
			arrsum[i][j] = arr1[i][j] + arr2[i][j];

	for (int i = 0; i < 3; ++i) {
		cout << arr1[i][0] << ' ' << arr1[i][1] << ' ' << arr1[i][2];
		if (i == 1)
			cout << " + ";
		else
			cout << "   ";
		cout << arr2[i][0] << ' ' << arr2[i][1] << ' ' << arr2[i][2];
		if (i == 1)
			cout << " = ";
		else
			cout << "   ";
		cout << arrsum[i][0] << ' ' << arrsum[i][1] << ' ' << arrsum[i][2];
		cout << endl;
	}
}

void PrintDiffOfArr(int arr1[3][3], int arr2[3][3]) {
	int arrdiff[3][3];

	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < 3; ++j)
			arrdiff[i][j] = arr1[i][j] - arr2[i][j];

	for (int i = 0; i < 3; ++i) {
		cout << arr1[i][0] << ' ' << arr1[i][1] << ' ' << arr1[i][2];
		if (i == 1)
			cout << " - ";
		else
			cout << "   ";
		cout << arr2[i][0] << ' ' << arr2[i][1] << ' ' << arr2[i][2];
		if (i == 1)
			cout << " = ";
		else
			cout << "   ";
		cout << arrdiff[i][0] << ' ' << arrdiff[i][1] << ' ' << arrdiff[i][2];
		cout << endl;
	}
}

void PrintMultiOfArr(int arr1[3][3], int arr2[3][3]) {
	int arrmulti[3][3];
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			arrmulti[i][j] = 0;
			for (int k = 0; k < 3; ++k) {
				arrmulti[i][j] += arr1[i][k] * arr2[k][j];
			}
		}
	}

	for (int i = 0; i < 3; ++i) {
		cout << arr1[i][0] << ' ' << arr1[i][1] << ' ' << arr1[i][2];
		if (i == 1)
			cout << " * ";
		else
			cout << "   ";
		cout << arr2[i][0] << ' ' << arr2[i][1] << ' ' << arr2[i][2];
		if (i == 1)
			cout << " = ";
		else
			cout << "   ";
		cout << arrmulti[i][0] << ' ' << arrmulti[i][1] << ' ' << arrmulti[i][2];
		cout << endl;
	}
}

void PrintDetOfArr(int arr1[3][3], int arr2[3][3]) {
	int arrdet1 = 0, arrdet2 = 0;;
	arrdet1 += ((arr1[0][0] * (arr1[1][1] * arr1[2][2] - arr1[1][2] * arr1[2][1])) + (arr1[0][1] * -1 * (arr1[1][0] * arr1[2][2] - arr1[1][2] * arr1[2][0])) + (arr1[0][2] * (arr1[1][0] * arr1[2][1] - arr1[1][1] * arr1[2][0])));
	arrdet2 += ((arr2[0][0] * (arr2[1][1] * arr2[2][2] - arr2[1][2] * arr2[2][1])) + (arr2[0][1] * -1 * (arr2[1][0] * arr2[2][2] - arr2[1][2] * arr2[2][0])) + (arr2[0][2] * (arr2[1][0] * arr2[2][1] - arr2[1][1] * arr2[2][0])));
	
	for (int i = 0; i < 3; ++i) {
		cout << arr1[i][0] << ' ' << arr1[i][1] << ' ' << arr1[i][2];
		if (i == 1)
			cout << " = " << arrdet1;
		cout << endl;
	}
	cout << endl;
	for (int i = 0; i < 3; ++i) {
		cout << arr2[i][0] << ' ' << arr2[i][1] << ' ' << arr2[i][2];
		if (i == 1)
			cout << " = " << arrdet2;
		cout << endl;
	}
}

void PrintTransArr(int arr1[3][3], int arr2[3][3]) {
	int arrtrans1[3][3], arrtrans2[3][3];
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			arrtrans1[j][i] = arr1[i][j];
			arrtrans2[j][i] = arr2[i][j];
		}
	}

	int arrdet1 = 0, arrdet2 = 0;
	arrdet1 += ((arrtrans1[0][0] * (arrtrans1[1][1] * arrtrans1[2][2] - arrtrans1[1][2] * arrtrans1[2][1])) + (arrtrans1[0][1] * -1 * (arrtrans1[1][0] * arrtrans1[2][2] - arrtrans1[1][2] * arrtrans1[2][0])) + (arrtrans1[0][2] * (arrtrans1[1][0] * arrtrans1[2][1] - arrtrans1[1][1] * arrtrans1[2][0])));
	arrdet2 += ((arrtrans2[0][0] * (arrtrans2[1][1] * arrtrans2[2][2] - arrtrans2[1][2] * arrtrans2[2][1])) + (arrtrans2[0][1] * -1 * (arrtrans2[1][0] * arrtrans2[2][2] - arrtrans2[1][2] * arrtrans2[2][0])) + (arrtrans2[0][2] * (arrtrans2[1][0] * arrtrans2[2][1] - arrtrans2[1][1] * arrtrans2[2][0])));

	for (int i = 0; i < 3; ++i) {
		cout << arr1[i][0] << ' ' << arr1[i][1] << ' ' << arr1[i][2];
		if (i == 1)
			cout << " -> ";
		else
			cout << "    ";
		cout << arrtrans1[i][0] << ' ' << arrtrans1[i][1] << ' ' << arrtrans1[i][2];
		if (i == 1)
			cout << " = " << arrdet1;
		cout << endl;
	}
	cout << endl;
	for (int i = 0; i < 3; ++i) {
		cout << arr2[i][0] << ' ' << arr2[i][1] << ' ' << arr2[i][2];
		if (i == 1)
			cout << " -> ";
		else
			cout << "    ";
		cout << arrtrans2[i][0] << ' ' << arrtrans2[i][1] << ' ' << arrtrans2[i][2];
		if (i == 1)
			cout << " = " << arrdet2;
		cout << endl;
	}
}

void PrintHArr(int arr1[3][3], int arr2[3][3]) {
	int arrH1[4][4] = { {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,1} }, arrH2[4][4] = { {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,1} };
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			arrH1[i][j] = arr1[i][j];
			arrH2[i][j] = arr2[i][j];
		}
	}
	int arrdet1 = 0, arrdet2 = 0;
	for (int i = 0; i < 4; ++i) {
		int temp[3][3];
		int r = 0, c = 0;
		for (int j = 1; j < 4; ++j) {
			for (int k = 0; k < 4; ++k) {
				if (k != i) {
					temp[r][c] = arrH1[j][k];
					++c;
				}
			}
			c = 0;
			++r;
		}
		arrdet1 += arrH1[0][i] * (i % 2 == 0 ? 1 : -1) * ((temp[0][0] * (temp[1][1] * temp[2][2] - temp[1][2] * temp[2][1])) + (temp[0][1] * -1 * (temp[1][0] * temp[2][2] - temp[1][2] * temp[2][0])) + (temp[0][2] * (temp[1][0] * temp[2][1] - temp[1][1] * temp[2][0])));
	}

	for (int i = 0; i < 4; ++i) {
		int temp[3][3];
		int r = 0, c = 0;
		for (int j = 1; j < 4; ++j) {
			for (int k = 0; k < 4; ++k) {
				if (k != i) {
					temp[r][c] = arrH2[j][k];
					++c;
				}
			}
			c = 0;
			++r;
		}
		arrdet2 += arrH2[0][i] * (i % 2 == 0 ? 1 : -1) * ((temp[0][0] * (temp[1][1] * temp[2][2] - temp[1][2] * temp[2][1])) + (temp[0][1] * -1 * (temp[1][0] * temp[2][2] - temp[1][2] * temp[2][0])) + (temp[0][2] * (temp[1][0] * temp[2][1] - temp[1][1] * temp[2][0])));
	}

	for (int i = 0; i < 4; ++i) {
		if (i != 3)
			cout << arr1[i][0] << ' ' << arr1[i][1] << ' ' << arr1[i][2];
		else
			cout << "     ";

		if (i == 1)
			cout << " -> ";
		else
			cout << "    ";
		cout << arrH1[i][0] << ' ' << arrH1[i][1] << ' ' << arrH1[i][2] << ' ' << arrH1[i][3];
		if (i == 1)
			cout << " = " << arrdet1;
		cout << endl;
	}
	cout << endl;
	for (int i = 0; i < 4; ++i) {
		if (i != 3)
			cout << arr2[i][0] << ' ' << arr2[i][1] << ' ' << arr2[i][2];
		else
			cout << "     ";

		if (i == 1)
			cout << " -> ";
		else
			cout << "    ";
		cout << arrH2[i][0] << ' ' << arrH2[i][1] << ' ' << arrH2[i][2] << ' ' << arrH2[i][3];
		if (i == 1)
			cout << " = " << arrdet2;
		cout << endl;
	}
}