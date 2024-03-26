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

bool valid_check_value(string s) {
	if (s.size() != 10 || s[0] != '0' || s[1] != 'x') {
		cout << "���� �߸� �ԷµǾ����ϴ�. 0x12345679 ���׷� �Է����ּ���\n";
		return false;
	}
	for (int i = 2; i < 10; i++) {
		if (!(s[i] >= '0' && s[i] <= '9')
			&& !(s[i] >= 'A' && s[i] <= 'Z')) {
			cout << "���� �߸� �ԷµǾ����ϴ�. 0x1234568 ���·� �Է����ּ���\n";
			return false;
		}
	}
	return true;
}

void write(vector<string>& words) {
	// ����ó��
	if (words.size() != 3) {
		cout << "���ڸ� ��Ȯ�� �Է��ϼ���. ex) write n value\n";
		return;
	}
	if (!valid_check_value(words[2])) return;

	// exe ���� Ŀ�ǵ� ����
	path_order = path + "W " + words[1] + " " + words[2];

	// �ý��� �Է�
	system(path_order.c_str());
}

void read(vector<string>& words, string n = "-1") {
	/*
	n��° �� -> result.txt ���� -> Shell���� ���
	*/
	if (n == "-1") {
		// ���� ó��
		if (words.size() != 2) {
			cout << "���ڸ� ��Ȯ�� �Է��ϼ���. ex) read n\n";
			return;
		}
		if (words[1] < "0" || words[1] > "99") {
			cout << "�߸��� LAB ��ȣ, 0~99�� �Է����ּ���.\n";
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
		cout << n << "��° ���� ����ֽ��ϴ�.";
		return;
	}

	cout << str << "\n";
}

void fullwrite(vector<string>& words) {
	// ����ó��
	if (words.size() != 2) {
		cout << "���ڸ� ��Ȯ�� �Է��ϼ���. ex) fullwrite value\n";
		return;
	}
	if (!valid_check_value(words[1])) return;

	path_order = path + "FW " + words[1];

	system(path_order.c_str());
}

bool fullread(vector<string>& words, string compare_s = "", int range = 0) {
	if (words.size() != 1) {
		cout << "���ڸ� ��Ȯ�� �Է��ϼ���. ex) fullread\n";
		return false;
	}
	path_order = path + "FR";

	system(path_order.c_str());
	string path_result = READ_FILE; // ��� ���� �ʿ�
	string str;
	ifstream file(path_result);

	int cnt = 0;
	if (!compare_s.empty()) {  // "testapp1" ��ɾ� ����
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

void testapp1(vector<string>& words, string value) {
	if (words.size() != 1) {
		cout << "���ڸ� ��Ȯ�� �Է��ϼ���. ex) testapp1\n";
		return;
	}
	if (!valid_check_value(value)) return;

	words.push_back(value);  // ���� �� ����
	fullwrite(words);
	vector<string> no_use_words;
	no_use_words.push_back(words[0]);
	if (!fullread(no_use_words, value))
		cout << "ReadCompare ����ġ\n";
	else
		cout << "ReadCompare ��ġ\n";
}

void testapp2(int n_repeat, int range, string value) {
	/*
	n_peat : ������ �ݺ��� Ƚ��
	range : LBA ���� ����
	value : ������ ��
	*/
	if (range < 1 || range > 99) {
		cout << "range ������ 0~99 �Դϴ�.\n";
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

	init(); // nand.txt, result.txt �ʱ�ȭ

	while (1) {
		getline(cin, s);  // Ŀ�ǵ� �Է�

		// ���� ���� ���ڿ� ���� -> words ���Ϳ� ����
		vector<string> words{};
		string_segment(s, words);

		if (words[0] == "exit") {
			if (words.size() != 1)
				cout << "���ڸ� ��Ȯ�� �Է��ϼ���. ex) exit\n";
			else
				break;
		}
		else if (words[0] == "help") {
			if (words.size() != 1)
				cout << "���ڸ� ��Ȯ�� �Է��ϼ���. ex) help\n";
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
			cout << "���� ������ fullwrite ���� ��, fullread ��ġ ���� Ȯ��\n";
			testapp1(words, "0x00000001");
		}
		else if (words[0] == "testapp2") {
			// 1. 0 ~ 5 �� LBA �� 0xAAAABBBB ������ �� 30�� Write�� �����Ѵ�
			cout << "0 ~ 5 �� LBA �� 0xAAAABBBB ������ �� 30�� Write�� ����\n";
			testapp2(30, 6, "0xAAAABBBB");

			// 2. 0 ~ 5 �� LBA �� 0x12345678 ������ 1 ȸ Over Write�� �����Ѵ�
			cout << "0 ~ 5 �� LBA �� 0x12345678 ������ 1 ȸ Over Write�� ����\n";
			testapp2(1, 6, "0x12345678");
		}
		else
			cout << "INVALID COMMAND\n";
	}
}