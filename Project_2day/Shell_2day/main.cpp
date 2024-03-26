#define _CRT_SECURE_NO_WARNINGS
#include<cstdlib>
#include<iostream>
#include<string>
#include<fstream>
#include<vector>
#include<ctime>
#include<filesystem>
#include <windows.h>
#include<algorithm>
using namespace std;

#define WRITE_FILE "nand.txt"
#define READ_FILE "result.txt"
#define BLOCK_SIZE 8
#define RUN_FILE "run_list.lst"

string path = "D:/김세준/_세메스/코테교육/_Project/semes_project/Project_2day/SSD_2day/Release/SSD_2day.exe ";
string path_order;
string file_search_path = "D:/김세준/_세메스/코테교육/_Project/semes_project/Project_2day/Shell_2day/";

char roption[400];

bool log_on_off = true;

class logger {
	/*
	funciton_name : 호출한 함수(수행한 명령) 이름
	logger_message : 성공 or 실패(에러요인)

	latest.log 파일 여부 확인 -> 10kb 넘을시 -> until_날짜_시간.log 저장
	-> 새로운 latest.log 생성 -> 기록(latest.log 맨 아랫줄에) -> print 출력

	기록 : [날짜 시간] fnction_name    : logger_message
	*/
private:
	string log_name;
public:
	logger() {
		log_name = "latest.log";
		init();
	}

	void init() {  // latest.log 초기화
		ofstream file(log_name);
		file.close();
	}
	bool check_capacity() {
		ifstream file(log_name);

		// 파일 포인터를 파일 끝으로 이동시킴
		file.seekg(0, std::ios::end);

		// 현재 파일 포인터 위치가 파일 크기입니다.
		std::streampos fileSize = file.tellg();

		file.close();

		if (fileSize > 10000)
			return false;
		return true;
	}

	void make_new_log() {
		time_t timer;
		struct tm* t;
		timer = time(NULL);
		t = localtime(&timer);

		string new_log_name;
		new_log_name = "until_" + to_string(t->tm_year % 100) + to_string(t->tm_mon + 1) + to_string(t->tm_mday) + "_"
			+ to_string(t->tm_hour) + "h_" + to_string(t->tm_min) + "m_"
			+ to_string(t->tm_sec) + "s.log";

		const char* oldName = log_name.c_str();
		const char* newName = new_log_name.c_str();
		/*cout << oldName << " to " << newName << "\n";
		cout << rename(oldName, newName) << "\n";*/
		if (rename(oldName, newName) == 0) {
			cout << "latset.log 용량이 10KB 초과하여 이름이 변경되었습니다.\n";

			check_possible_zip();
		}
	}

	void check_possible_zip() {
		// 폴더 내 파일들 중에 .log 불러와서 latset.log를 제외하고 2개 존재시 -> 최근 것을 zip 생성
		string strDirName = file_search_path;
		string str = strDirName + "*.log";

		WIN32_FIND_DATAA data;
		HANDLE hFind;
		vector<string> vecFiles;

		if ((hFind = FindFirstFileA(str.c_str(), &data)) != INVALID_HANDLE_VALUE)
		{
			do {
				vecFiles.emplace_back(string(data.cFileName));
			} while (FindNextFileA(hFind, &data) != 0);
			FindClose(hFind);
		}

		if (vecFiles.size() >= 3) { // latest.log, 최신1, 최신2 존재시 가장 최신 -> zip 
			sort(vecFiles.begin(), vecFiles.end(), greater<string>());  // 내림차순 정렬
			const char* oldName = vecFiles[0].c_str();

			string zip_name;
			zip_name = vecFiles[0].substr(0, vecFiles[0].find('.'));
			zip_name += ".zip";

			const char* newName = zip_name.c_str();
			if (rename(oldName, newName) == 0)
				cout << "zip 파일 생성 : " << oldName << " -> " << newName << "\n";
		}
	}

	void forward(string function_name, string logger_message) {
		if (!check_capacity())
			make_new_log();  // 새로운 latest.log 생성 및 zip 가능여부 판단 후 생성

		// log에 이어쓰기
		ofstream file(log_name, ios_base::app);

		time_t timer;
		struct tm* t;
		timer = time(NULL);
		t = localtime(&timer);

		string total_log_message = "[" + to_string(t->tm_year % 100) + "." + to_string(t->tm_mon + 1) + "." + to_string(t->tm_mday) + " "
			+ to_string(t->tm_hour) + ":" + to_string(t->tm_min) + "] " + function_name + "    : " + logger_message;
		cout << total_log_message << "\n";

		file << total_log_message << "\n";
		file.close();
	}
};

void init(string filename) {
	ofstream ofs(filename);
	ofs.write(roption, 400);
	ofs.close();
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

bool valid_check_value(string s, logger& logs) {
	if (s.size() != 10 || s[0] != '0' || s[1] != 'x') {
		logs.forward("valid_check_value()", "값이 잘못 입력되었습니다. 0x12345679 형테로 입력해주세요");
		return false;
	}
	for (int i = 2; i < 10; i++) {
		if (!(s[i] >= '0' && s[i] <= '9')
			&& !(s[i] >= 'A' && s[i] <= 'Z')) {
			logs.forward("valid_check_value()", "값이 잘못 입력되었습니다. 0x1234568 형태로 입력해주세요");
			return false;
		}
	}
	return true;
}

void write(vector<string>& words, logger& logs) {
	if (log_on_off) {
		// 예외처리
		if (words.size() != 3) {
			logs.forward("write()", "인자를 정확히 입력하세요. ex) write n value");
			return;
		}
		if (!valid_check_value(words[2], logs)) return;
	}

	// exe 실행 커맨드 생성
	path_order = path + "W " + words[1] + " " + words[2];

	// 시스템 입력
	system(path_order.c_str());

	if (log_on_off)
		logs.forward("write()", "데이터 입력 완료");
}

bool read(vector<string>& words, logger& logs, string n = "", string compare_s = "") {
	/*
	n번째 값 -> result.txt 저장 -> Shell에서 출력
	*/
	if (n == "" && log_on_off) {
		// 예외 처리
		if (words.size() != 2) {
			logs.forward("read()", "인자를 정확히 입력하세요. ex) read n");
			return false;
		}
		if (stoi(words[1]) < 0 || stoi(words[1]) > 99) {
			logs.forward("read()", "잘못된 LAB 번호, 0~99를 입력해주세요.");
			return false;
		}
		n = words[1];
	}
	path_order = path + "R " + n;

	system(path_order.c_str());
	string path_result = READ_FILE;
	string str;
	ifstream file(path_result);
	getline(file, str);
	if (str == "0x00000000" && log_on_off) {
		logs.forward("read()", "해당 값은 비어있습니다.");
		return false;
	}

	if (log_on_off) {
		cout << str << "\n";
		logs.forward("read()", "데이터 읽기 완료");
		return true;
	}
	else {
		if (str == compare_s)
			return true;
		else false;
	}
}

void fullwrite(vector<string>& words, logger& logs) {
	if (log_on_off) {
		// 예외처리
		if (words.size() != 2) {
			logs.forward("fullwrite()", "인자를 정확히 입력하세요. ex) fullwrite value");
			return;
		}
		if (!valid_check_value(words[1], logs)) return;
	}

	path_order = path + "FW " + words[1];

	system(path_order.c_str());

	if (log_on_off)
		logs.forward("fullwrite()", "데이터 전체 입력 완료");
}

bool fullread(vector<string>& words, logger& logs, string compare_s = "", int range = 0) {
	if (log_on_off) {
		if (words.size() != 1) {
			logs.forward("fullread()", "인자를 정확히 입력하세요. ex) fullread");
			return false;
		}
	}
	path_order = path + "FR";

	system(path_order.c_str());
	string path_result = READ_FILE; // 경로 설정 필요
	string str;
	ifstream file(path_result);

	int cnt = 0;
	if (compare_s != "") {  // "testapp1" 명령어 수행
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
	if (log_on_off)
		logs.forward("fullread()", "데이터 전체 읽기 완료");
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

bool testapp1(vector<string>& words, string value, logger& logs) {
	if (log_on_off) {
		if (words.size() != 1) {
			logs.forward("testapp1()", "인자를 정확히 입력하세요. ex) testapp1");
			return false;
		}
		if (!valid_check_value(value, logs)) return false;
	}

	words.push_back(value);  // 임의 값 수정
	fullwrite(words, logs);
	vector<string> no_use_words;
	no_use_words.push_back(words[0]);
	if (!fullread(no_use_words, logs, value)) {
		if (log_on_off)
			logs.forward("testapp1()", "ReadCompare 불일치");
		return false;
	}
	else {
		if (log_on_off)
			logs.forward("testapp1()", "ReadCompare 일치");
		return true;
	}
}

void testapp2(int n_repeat, int range, string value, logger& logs) {
	/*
	n_peat : 저장을 반복할 횟수
	range : LBA 저장 범위
	value : 저장할 값
	*/
	if (range < 1 || range > 99) {
		logs.forward("testapp2()", "range 범위는 0~99 입니다.");
		return;
	}
	if (!valid_check_value(value, logs)) return;

	for (int i = 0; i < n_repeat; i++) {
		for (int j = 0; j < range; j++) {
			vector<string> words_2;
			words_2.push_back("no_use");
			words_2.push_back(to_string(j));
			words_2.push_back(value);
			write(words_2, logs);
		}
	}
	for (int i = 0; i < range; i++) {
		vector<string> buf;
		read(buf, logs, to_string(i));
	}
	logs.forward("testapp2()", "수행완료");
}

void erase(vector<string>& words, logger& logs) {
	if (words.size() != 3 && log_on_off) {
		logs.forward("erase()", "입력 인자 수가 잘못되었습니다");
		return;
	}
	int arg_second = stoi(words[1]);
	int arg_third = stoi(words[2]);

	if (log_on_off) {
		if (arg_second < 0 || arg_second > 99) {
			logs.forward(words[0], "잘못된 LAB 번호, 0~99를 입력해주세요.");
			return;
		}
		if (arg_third < 0 || arg_third > 99) {
			logs.forward(words[0], "잘못된 LAB 번호, 0~99를 입력해주세요.");
			return;
		}
	}

	if (words[0] == "erase") {
		if (arg_second + arg_third > 99 && log_on_off) {
			logs.forward("erase()", "SIZE가 LBA 전체 범위를 벗어납니다.");
			return;
		}
		path_order = path + "E " + words[1] + " " + words[2];
		system(path_order.c_str());
		if (log_on_off)
			logs.forward("erase()", "삭제 완료");
	}
	else {
		if (arg_second > arg_third && log_on_off) {
			logs.forward("eras_rage()", "start LBA < end LBA 가 되어야 합니다.");
			return;
		}
		path_order = path + "E " + words[1] + " " + to_string(arg_third - arg_second);
		system(path_order.c_str());

		if (log_on_off)
			logs.forward("erase_range()", "삭제 완료");
	}

}

void runner(int argv, char* argc[]) {
	/*
	< 카테고리 >
	FullWriteReadCompare : 특정값 전체 데이터에 저장 후, 읽으면서 비교
	Write10AndCompare : 10번에 write 한 후에 비교
	WriteReadEraseRead : 특정 LBA에 임의의 값 저장 -> Read 확인 -> Erase -> Erase 확인
	*/
	log_on_off = false;

	ifstream file(RUN_FILE);
	string str;

	logger logs;
	while (getline(file, str)) {  // 라인마다 받아옴
		if (str == "FullWriteReadCompare") {
			cout << "FullWriteReadCompare  ---  Run...";
			vector<string> words;
			words.push_back("not_use");
			if (testapp1(words, "0x12345678", logs))
				cout << "Pass\n";
			else
				cout << "Fail\n";
		}
		else if (str == "Write10AndCompare") {
			cout << "Write10AndCompare  ---  Run...";
			vector<string> words;
			words.push_back("no_use");
			words.push_back("10");
			words.push_back("0x99999999");
			write(words, logs);

			if (read(words, logs, "10", "0x99999999"))
				cout << "Pass\n";
			else
				cout << "Fail\n";
		}
		else if (str == "WriteReadEraseRead") {
			cout << "WriteReadEraseRead  ---  Run...";
			vector<string> words;
			words.push_back("no_use");
			words.push_back("7");
			words.push_back("0x77777777");
			write(words, logs);

			if (!read(words, logs, "7", "0x77777777")) {
				cout << "Fail\n";
				continue;
			}
			words.clear();
			words.push_back("erase");
			words.push_back("7");
			words.push_back("8");
			erase(words, logs);

			if (read(words, logs, "7", "0x00000000"))
				cout << "Pass\n";
			else
				cout << "Fail\n";
		}
	}
}

int main(int argv, char* argc[]) {
	string s;

	init(WRITE_FILE); // nand.txt, result.txt 초기화
	logger logs;

	if (argv == 2)  // runner 수행
		runner(argv, argc);
	else {
		while (1) {
			getline(cin, s);  // 커맨드 입력

			// 공백 기준 문자열 분할 -> words 벡터에 저장
			vector<string> words{};
			string_segment(s, words);

			if (words[0] == "exit") {
				if (words.size() != 1) {
					logs.forward("exit()", "인자를 정확히 입력하세요. ex) exit");
					continue;
				}
				else {
					logs.forward("exit()", "프로그램을 종료합니다.");
					break;
				}
			}
			else if (words[0] == "help") {
				if (words.size() != 1) {
					logs.forward("help()", "인자를 정확히 입력하세요. ex) help");
					continue;
				}
				else {
					logs.forward("help()", "다음과 같습니다");
					help();
				}
			}

			// Write or Read
			else if (words[0] == "write")
				write(words, logs);
			else if (words[0] == "read")
				read(words, logs);
			else if (words[0] == "fullwrite")
				fullwrite(words, logs);
			else if (words[0] == "fullread")
				fullread(words, logs);
			else if (words[0] == "testapp1")
				testapp1(words, "0x00000001", logs);
			else if (words[0] == "testapp2") {
				// 1. 0 ~ 5 번 LBA 에 0xAAAABBBB 값으로 총 30번 Write를 수행한다
				testapp2(30, 6, "0xAAAABBBB", logs);

				// 2. 0 ~ 5 번 LBA 에 0x12345678 값으로 1 회 Over Write를 수행한다
				cout << "0 ~ 5 번 LBA 에 0x12345678 값으로 1 회 Over Write를 수행\n";
				testapp2(1, 6, "0x12345678", logs);
			}
			else if (words[0] == "erase") {
				erase(words, logs);
			}
			else
				cout << "INVALID COMMAND\n";
		}
	}
}