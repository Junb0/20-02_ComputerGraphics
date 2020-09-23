#include<iostream>
#include<fstream>
#include<string>
#include<vector>

using namespace std;

int main() {
	string fname;
	string full;
	vector<string> words;
	vector<string> nums;
	string temp;
	int size;

	cout << "input data file name : ";
	cin >> fname;
	ifstream in(fname);
	if (in.is_open() == false) {
		cout << fname << " 파일을 찾을 수 없습니다.";
		return 0;
	}

	while (!in.eof()) {
		in >> temp;
		words.push_back(temp);
	}

	in.seekg(0, ios::end);
	size = in.tellg();
	full.resize(size);
	in.seekg(0, ios::beg);
	in.read(&full[0], size);

	for (string s : words) {
		for (char c : s) {
			if (c >= '0' && c <= '9') {
				nums.push_back(s);
				break;
			}
		}
	}

	cout << full << endl << endl;
	cout << "word count : " << words.size() - nums.size() << endl;
	cout << "number count : " << nums.size() << endl;
	cout << "numbers : ";
	for (string s : nums) {
		cout << s << ' ';
	}
	cout << endl;
}