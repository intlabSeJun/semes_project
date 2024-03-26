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

string path = "D:/�輼��/_���޽�/���ױ���/_Project/semes_project/Project_2day/SSD_2day/Release/SSD_2day.exe ";
string path_order;
string file_search_path = "D:/�輼��/_���޽�/���ױ���/_Project/semes_project/Project_2day/Shell_2day/";

char roption[400];

bool log_on_off = true;

class logger {
	/*
	funciton_name : ȣ���� �Լ�(������ ���) �̸�
	logger_message : ���� or ����(��������)

	latest.log ���� ���� Ȯ�� -> 10kb ������ -> until_��¥_�ð�.log ����
	-> ���ο� latest.log ���� -> ���(latest.log �� �Ʒ��ٿ�) -> print ���

	��� : [��¥ �ð�] fnction_name    : logger_message
	*/
private:
	string log_name;
public:
	logger() {
		log_name = "latest.log";
		init();
	}

	void init() {  // latest.log �ʱ�ȭ
		ofstream file(log_name);
		file.close();
	}
	bool check_capacity() {
		ifstream file(log_name);

		// ���� �����͸� ���� ������ �̵���Ŵ
		file.seekg(0, std::ios::end);

		// ���� ���� ������ ��ġ�� ���� ũ���Դϴ�.
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
			cout << "latset.log �뷮�� 10KB �ʰ��Ͽ� �̸��� ����Ǿ����ϴ�.\n";

			check_possible_zip();
		}
	}

	void check_possible_zip() {
		// ���� �� ���ϵ� �߿� .log �ҷ��ͼ� latset.log�� �����ϰ� 2�� ����� -> �ֱ� ���� zip ����
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

		if (vecFiles.size() >= 3) { // latest.log, �ֽ�1, �ֽ�2 ����� ���� �ֽ� -> zip 
			sort(vecFiles.begin(), vecFiles.end(), greater<string>());  // �������� ����
			const char* oldName = vecFiles[0].c_str();

			string zip_name;
			zip_name = vecFiles[0].substr(0, vecFiles[0].find('.'));
			zip_name += ".zip";

			const char* newName = zip_name.c_str();
			if (rename(oldName, newName) == 0)
				cout << "zip ���� ���� : " << oldName << " -> " << newName << "\n";
		}
	}

	void forward(string function_name, string logger_message) {
		if (!check_capacity())
			make_new_log();  // ���ο� latest.log ���� �� zip ���ɿ��� �Ǵ� �� ����

		// log�� �̾��
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
	// �� ��ɾ� ��� ��� ���
	cout << "help                �� ��ɾ� ��� ��� ���\n";
	cout << "write n content     SSD�� n���� content�� �����մϴ�.\n";
	cout << "read n              SSD�� n���� content�� �ҷ��ɴϴ�.\n";
	cout << "fullwrite content   SSD ��ü�� conent�� �������ϴ�.\n";
	cout << "fullread            SSD ��ü ���� ȭ�鿡 ����մϴ�.\n";
	cout << "exit                Shell �����մϴ�.\n";
	cout << "testapp1            fullwrite -> fullread �ùٸ��� Ȯ���մϴ�.\n";
	cout << "testapp2            0~5�� LBA�� Ư�� ������ 30�� write ���� �� OverWrite ���� -> read�� ���󿩺� Ȯ���մϴ�.\n\n";
}

bool valid_check_value(string s, logger& logs) {
	if (s.size() != 10 || s[0] != '0' || s[1] != 'x') {
		logs.forward("valid_check_value()", "���� �߸� �ԷµǾ����ϴ�. 0x12345679 ���׷� �Է����ּ���");
		return false;
	}
	for (int i = 2; i < 10; i++) {
		if (!(s[i] >= '0' && s[i] <= '9')
			&& !(s[i] >= 'A' && s[i] <= 'Z')) {
			logs.forward("valid_check_value()", "���� �߸� �ԷµǾ����ϴ�. 0x1234568 ���·� �Է����ּ���");
			return false;
		}
	}
	return true;
}

void write(vector<string>& words, logger& logs) {
	if (log_on_off) {
		// ����ó��
		if (words.size() != 3) {
			logs.forward("write()", "���ڸ� ��Ȯ�� �Է��ϼ���. ex) write n value");
			return;
		}
		if (!valid_check_value(words[2], logs)) return;
	}

	// exe ���� Ŀ�ǵ� ����
	path_order = path + "W " + words[1] + " " + words[2];

	// �ý��� �Է�
	system(path_order.c_str());

	if (log_on_off)
		logs.forward("write()", "������ �Է� �Ϸ�");
}

bool read(vector<string>& words, logger& logs, string n = "", string compare_s = "") {
	/*
	n��° �� -> result.txt ���� -> Shell���� ���
	*/
	if (n == "" && log_on_off) {
		// ���� ó��
		if (words.size() != 2) {
			logs.forward("read()", "���ڸ� ��Ȯ�� �Է��ϼ���. ex) read n");
			return false;
		}
		if (stoi(words[1]) < 0 || stoi(words[1]) > 99) {
			logs.forward("read()", "�߸��� LAB ��ȣ, 0~99�� �Է����ּ���.");
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
		logs.forward("read()", "�ش� ���� ����ֽ��ϴ�.");
		return false;
	}

	if (log_on_off) {
		cout << str << "\n";
		logs.forward("read()", "������ �б� �Ϸ�");
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
		// ����ó��
		if (words.size() != 2) {
			logs.forward("fullwrite()", "���ڸ� ��Ȯ�� �Է��ϼ���. ex) fullwrite value");
			return;
		}
		if (!valid_check_value(words[1], logs)) return;
	}

	path_order = path + "FW " + words[1];

	system(path_order.c_str());

	if (log_on_off)
		logs.forward("fullwrite()", "������ ��ü �Է� �Ϸ�");
}

bool fullread(vector<string>& words, logger& logs, string compare_s = "", int range = 0) {
	if (log_on_off) {
		if (words.size() != 1) {
			logs.forward("fullread()", "���ڸ� ��Ȯ�� �Է��ϼ���. ex) fullread");
			return false;
		}
	}
	path_order = path + "FR";

	system(path_order.c_str());
	string path_result = READ_FILE; // ��� ���� �ʿ�
	string str;
	ifstream file(path_result);

	int cnt = 0;
	if (compare_s != "") {  // "testapp1" ��ɾ� ����
		while (getline(file, str)) {
			if (str != compare_s)  // �Է� �ȵǾ��� ��
				return false;
			if (range != 0) {
				cnt++;
				if (cnt == range)  // Ư�� �������� ����
					return true;
			}
		}
	}
	else {
		while (getline(file, str))
			cout << str << "\n";
	}
	if (log_on_off)
		logs.forward("fullread()", "������ ��ü �б� �Ϸ�");
	return true;
}

void string_segment(string& s, vector<string>& words) {
	if (words.size() > 3) {
		cout << "���ڰ� �ʹ� �����ϴ�. help ��ɾ �����ϼ���.\n";
		return;
	}
	string s2 = s;
	string space_delimiter = " ";

	// ���� ���� ���ڿ� ���� -> words ���Ϳ� ����
	size_t pos = 0;
	if (s2.find(space_delimiter) == string::npos) // ���� ���� ��
		words.push_back(s);
	else {  // ���� ���� ��
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
			logs.forward("testapp1()", "���ڸ� ��Ȯ�� �Է��ϼ���. ex) testapp1");
			return false;
		}
		if (!valid_check_value(value, logs)) return false;
	}

	words.push_back(value);  // ���� �� ����
	fullwrite(words, logs);
	vector<string> no_use_words;
	no_use_words.push_back(words[0]);
	if (!fullread(no_use_words, logs, value)) {
		if (log_on_off)
			logs.forward("testapp1()", "ReadCompare ����ġ");
		return false;
	}
	else {
		if (log_on_off)
			logs.forward("testapp1()", "ReadCompare ��ġ");
		return true;
	}
}

void testapp2(int n_repeat, int range, string value, logger& logs) {
	/*
	n_peat : ������ �ݺ��� Ƚ��
	range : LBA ���� ����
	value : ������ ��
	*/
	if (range < 1 || range > 99) {
		logs.forward("testapp2()", "range ������ 0~99 �Դϴ�.");
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
	logs.forward("testapp2()", "����Ϸ�");
}

void erase(vector<string>& words, logger& logs) {
	if (words.size() != 3 && log_on_off) {
		logs.forward("erase()", "�Է� ���� ���� �߸��Ǿ����ϴ�");
		return;
	}
	int arg_second = stoi(words[1]);
	int arg_third = stoi(words[2]);

	if (log_on_off) {
		if (arg_second < 0 || arg_second > 99) {
			logs.forward(words[0], "�߸��� LAB ��ȣ, 0~99�� �Է����ּ���.");
			return;
		}
		if (arg_third < 0 || arg_third > 99) {
			logs.forward(words[0], "�߸��� LAB ��ȣ, 0~99�� �Է����ּ���.");
			return;
		}
	}

	if (words[0] == "erase") {
		if (arg_second + arg_third > 99 && log_on_off) {
			logs.forward("erase()", "SIZE�� LBA ��ü ������ ����ϴ�.");
			return;
		}
		path_order = path + "E " + words[1] + " " + words[2];
		system(path_order.c_str());
		if (log_on_off)
			logs.forward("erase()", "���� �Ϸ�");
	}
	else {
		if (arg_second > arg_third && log_on_off) {
			logs.forward("eras_rage()", "start LBA < end LBA �� �Ǿ�� �մϴ�.");
			return;
		}
		path_order = path + "E " + words[1] + " " + to_string(arg_third - arg_second);
		system(path_order.c_str());

		if (log_on_off)
			logs.forward("erase_range()", "���� �Ϸ�");
	}

}

void runner(int argv, char* argc[]) {
	/*
	< ī�װ� >
	FullWriteReadCompare : Ư���� ��ü �����Ϳ� ���� ��, �����鼭 ��
	Write10AndCompare : 10���� write �� �Ŀ� ��
	WriteReadEraseRead : Ư�� LBA�� ������ �� ���� -> Read Ȯ�� -> Erase -> Erase Ȯ��
	*/
	log_on_off = false;

	ifstream file(RUN_FILE);
	string str;

	logger logs;
	while (getline(file, str)) {  // ���θ��� �޾ƿ�
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

	init(WRITE_FILE); // nand.txt, result.txt �ʱ�ȭ
	logger logs;

	if (argv == 2)  // runner ����
		runner(argv, argc);
	else {
		while (1) {
			getline(cin, s);  // Ŀ�ǵ� �Է�

			// ���� ���� ���ڿ� ���� -> words ���Ϳ� ����
			vector<string> words{};
			string_segment(s, words);

			if (words[0] == "exit") {
				if (words.size() != 1) {
					logs.forward("exit()", "���ڸ� ��Ȯ�� �Է��ϼ���. ex) exit");
					continue;
				}
				else {
					logs.forward("exit()", "���α׷��� �����մϴ�.");
					break;
				}
			}
			else if (words[0] == "help") {
				if (words.size() != 1) {
					logs.forward("help()", "���ڸ� ��Ȯ�� �Է��ϼ���. ex) help");
					continue;
				}
				else {
					logs.forward("help()", "������ �����ϴ�");
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
				// 1. 0 ~ 5 �� LBA �� 0xAAAABBBB ������ �� 30�� Write�� �����Ѵ�
				testapp2(30, 6, "0xAAAABBBB", logs);

				// 2. 0 ~ 5 �� LBA �� 0x12345678 ������ 1 ȸ Over Write�� �����Ѵ�
				cout << "0 ~ 5 �� LBA �� 0x12345678 ������ 1 ȸ Over Write�� ����\n";
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