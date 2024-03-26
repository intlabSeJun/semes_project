#define _CRT_SECURE_NO_WARNINGS
#include<cstdlib>
#include<iostream>
#include<string>
#include<fstream>
#include<vector>
using namespace std;

#define WRITE_FILE "nand.txt"
#define READ_FILE "result.txt"
#define BLOCK_SIZE 8

string path = "C:/Users/807-5/source/repos/SSD2/x64/Release/SSD2.exe ";
string path_order;

void init() {
	ofstream ifs(WRITE_FILE);

	for (char i = 0; i < 100; ++i) {
		char f = (i / 10) % 10 + '0';
		ifs << f;
		f = (i % 10) + '0';
		ifs << f;
		for (int i = 0; i < 8; i++) ifs << '0';
		ifs << '\n';
	}
	ifs.close();

	ofstream fout(READ_FILE);
	fout.close();
}

void help() {
	// 각 명령어 사용 방법 출력
	cout << "help                각 명령어 사용 방법 출력\n";
	cout << "write n content     SSD의 n번에 content를 저장합니다.\n";
	cout << "read n              SSD의 n번의 content를 불러옵니다.\n";
	cout << "fullwrite content   SSD 전체에 conent가 쓰여집니다.\n";
	cout << "fullread            SSD 전체 값을 화면에 출력합니다.\n";
	cout << "exit                Shell 종료합니다.\n";
	cout << "testapp1            fullwrite -> fullread 올바른지 확인합니다.\n";
	cout << "testapp2            0~5번 LBA에 특정 값으로 30번 write 수행 및 OverWrite 수행 -> read시 정상여부 확인합니다.\n\n";
}

bool valid_check_value(string s) {
	if (s.size() != 10 || s[0] != '0' || s[1] != 'x') {
		cout << "값이 잘못 입력되었습니다. 0x12345679 형테로 입력해주세요\n";
		return false;
	}
	for (int i = 2; i < 10; i++) {
		if (!(s[i] >= '0' && s[i] <= '9')
			&& !(s[i] >= 'A' && s[i] <= 'Z')) {
			cout << "값이 잘못 입력되었습니다. 0x1234568 형태로 입력해주세요\n";
			return false;
		}
	}
	return true;
}

void write(vector<string>& words) {
	// 예외처리
	if (words.size() != 3) {
		cout << "인자를 정확히 입력하세요. ex) write n value\n";
		return;
	}
	if (!valid_check_value(words[2])) return;

	// exe 실행 커맨드 생성
	path_order = path + "W " + words[1] + " " + words[2];

	// 시스템 입력
	system(path_order.c_str());
}

void read(vector<string>& words, string n = "-1") {
	/*
	n번째 값 -> result.txt 저장 -> Shell에서 출력
	*/
	if (n == "-1") {
		// 예외 처리
		if (words.size() != 2) {
			cout << "인자를 정확히 입력하세요. ex) read n\n";
			return;
		}
		if (words[1] < "0" || words[1] > "99") {
			cout << "잘못된 LAB 번호, 0~99를 입력해주세요.\n";
			return;
		}
		n = words[1];
	}
	path_order = path + "R " + n;

	system(path_order.c_str());
	string path_result = READ_FILE;
	string str;
	ifstream file(path_result);
	getline(file, str);
	if (str == "0x00000000") {
		cout << n << "번째 값은 비어있습니다.";
		return;
	}

	cout << str << "\n";
}

void fullwrite(vector<string>& words) {
	// 예외처리
	if (words.size() != 2) {
		cout << "인자를 정확히 입력하세요. ex) fullwrite value\n";
		return;
	}
	if (!valid_check_value(words[1])) return;

	path_order = path + "FW " + words[1];

	system(path_order.c_str());
}

bool fullread(vector<string>& words, string compare_s = "", int range = 0) {
	if (words.size() != 1) {
		cout << "인자를 정확히 입력하세요. ex) fullread\n";
		return false;
	}
	path_order = path + "FR";

	system(path_order.c_str());
	string path_result = READ_FILE; // 경로 설정 필요
	string str;
	ifstream file(path_result);

	int cnt = 0;
	if (!compare_s.empty()) {  // "testapp1" 명령어 수행
		while (getline(file, str)) {
			if (str != compare_s)  // 입력 안되었을 시
				return false;
			if (range != 0) {
				cnt++;
				if (cnt == range)  // 특정 범위까지 수행
					return true;
			}
		}
	}
	else {
		while (getline(file, str))
			cout << str << "\n";
	}
	return true;
}

void string_segment(string& s, vector<string>& words) {
	if (words.size() > 3) {
		cout << "인자가 너무 많습니다. help 명령어를 참조하세요.\n";
		return;
	}
	string s2 = s;
	string space_delimiter = " ";

	// 공백 기준 문자열 분할 -> words 벡터에 저장
	size_t pos = 0;
	if (s2.find(space_delimiter) == string::npos) // 공백 없을 시
		words.push_back(s);
	else {  // 공백 있을 시
		while ((pos = s2.find(space_delimiter)) != string::npos) {
			words.push_back(s2.substr(0, pos));
			s2.erase(0, pos + space_delimiter.length());
		}
		pos = s.rfind(space_delimiter);
		words.push_back(s.substr(pos + 1, s.length() - 1));
	}
}

void testapp1(vector<string>& words, string value) {
	if (words.size() != 1) {
		cout << "인자를 정확히 입력하세요. ex) testapp1\n";
		return;
	}
	if (!valid_check_value(value)) return;

	words.push_back(value);  // 임의 값 수정
	fullwrite(words);
	vector<string> no_use_words;
	no_use_words.push_back(words[0]);
	if (!fullread(no_use_words, value))
		cout << "ReadCompare 불일치\n";
	else
		cout << "ReadCompare 일치\n";
}

void testapp2(int n_repeat, int range, string value) {
	/*
	n_peat : 저장을 반복할 횟수
	range : LBA 저장 범위
	value : 저장할 값
	*/
	if (range < 1 || range > 99) {
		cout << "range 범위는 0~99 입니다.\n";
		return;
	}
	if (!valid_check_value(value)) return;

	for (int i = 0; i < n_repeat; i++) {
		for (int j = 0; j < range; j++) {
			vector<string> words_2;
			words_2.push_back("no_use");
			words_2.push_back(to_string(j));
			words_2.push_back(value);
			write(words_2);
		}
	}
	for (int i = 0; i < range; i++) {
		vector<string> buf;
		read(buf, to_string(i));
	}
}

int main() {
	string s;

	init(); // nand.txt, result.txt 초기화

	while (1) {
		getline(cin, s);  // 커맨드 입력

		// 공백 기준 문자열 분할 -> words 벡터에 저장
		vector<string> words{};
		string_segment(s, words);

		if (words[0] == "exit") {
			if (words.size() != 1)
				cout << "인자를 정확히 입력하세요. ex) exit\n";
			else
				break;
		}
		else if (words[0] == "help") {
			if (words.size() != 1)
				cout << "인자를 정확히 입력하세요. ex) help\n";
			else
				help();
		}

		// Write or Read
		else if (words[0] == "write")
			write(words);
		else if (words[0] == "read")
			read(words);
		else if (words[0] == "fullwrite")
			fullwrite(words);
		else if (words[0] == "fullread")
			fullread(words);
		else if (words[0] == "testapp1") {
			cout << "임의 값으로 fullwrite 수행 후, fullread 일치 여부 확인\n";
			testapp1(words, "0x00000001");
		}
		else if (words[0] == "testapp2") {
			// 1. 0 ~ 5 번 LBA 에 0xAAAABBBB 값으로 총 30번 Write를 수행한다
			cout << "0 ~ 5 번 LBA 에 0xAAAABBBB 값으로 총 30번 Write를 수행\n";
			testapp2(30, 6, "0xAAAABBBB");

			// 2. 0 ~ 5 번 LBA 에 0x12345678 값으로 1 회 Over Write를 수행한다
			cout << "0 ~ 5 번 LBA 에 0x12345678 값으로 1 회 Over Write를 수행\n";
			testapp2(1, 6, "0x12345678");
		}
		else
			cout << "INVALID COMMAND\n";
	}
}