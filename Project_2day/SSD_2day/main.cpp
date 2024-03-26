#include<iostream>
#include<fstream>
#include<string>
#include<map>
#include<set>

#define WRITE_FILE "nand.txt"
#define READ_FILE "result.txt"
#define BUFFER "buffer.txt"
#define MAX_ST 400

using namespace std;

unsigned char mkh(char data);
unsigned char mkhex(char data[2]);
char mkc_u(unsigned char data);
char mkc_d(unsigned char data);
void write_ssd(string filename);
void read_ssd(string filename);
void read(unsigned short memloc);
void full_read();
void full_write(char data[10]);
void write_result(unsigned short memloc, unsigned char memlen, string filename);
void init_buffer();
void write_buffer();
void buffer_write_ssd();
void buffer_write_roption(unsigned short memloc, string data);
unsigned char read_buffer();
void stack_buffer_erase(unsigned short memloc, unsigned char memlen);
void stack_buffer_write(unsigned short memloc, string data);
// nand 정보값 임시 저장용 배열
char roption[MAX_ST];
unsigned char buf_size;

map<unsigned short, string> buffer;
set<unsigned short> addr;
// 문자에 해당하는 hex값 반환
unsigned char mkh(char data) {
    if (data <= 'F' && data >= 'A') return data - 'A' + 10;
    else return data - '0';
}
// 문자 2개의 hex값을 합쳐 1byte 크기로 반환
unsigned char mkhex(char data[2]) {
    unsigned char ret = (mkh(data[0]) << 4) + mkh(data[1]);
    return ret;
}
// 1byte 상위 4bit char 형태 반환
char mkc_u(unsigned char data) {
    data = data >> 4;
    if (data >= 10) return 'A' + data - 10;
    else return '0' + data;
}
// 1byte 하위 4bit char 형태 반환
char mkc_d(unsigned char data) {
    data %= 16;
    if (data >= 10) return 'A' + data - 10;
    else return '0' + data;
}
// roption 배열에 nand 정보 가져옴
void read_ssd(string filename) {
    ifstream ifs(filename);
    ifs.read(roption, MAX_ST);
    ifs.close();
}
// roption 배열 정보 nand에 저장
void write_ssd(string filename) {
    ofstream ofs(filename);
    ofs.write(roption, MAX_ST);
    ofs.close();
}
// read 내용 result에 update
void write_result(unsigned short memloc, unsigned char memlen, string filename) {
    ofstream ofs(filename);
    string s;
    for (unsigned char i = 0; i < memlen; i++) {
        s = "0x";
        for (unsigned char j = 0; j < 4; j++) {
            s += mkc_u(roption[(memloc + i) * 4 + j]);
            s += mkc_d(roption[(memloc + i) * 4 + j]);
        }
        ofs << s << endl;
    }
    ofs.close();
}
// 특정 주소 정보값 읽어 result.txt에 저장
void read(unsigned short memloc) {
    if (addr.find(memloc) == addr.end()) {
        ifstream ifs(WRITE_FILE);
        ifs.seekg(memloc * 4, ios::beg);
        ifs.read(&roption[memloc * 4], 4);
        ifs.close();
    }
    else buffer_write_roption(memloc, buffer[memloc]);
    write_result(memloc, 1, READ_FILE);
}
// 전체 주소 정보값 읽어 result.txt에 저장
void full_read() {
    read_ssd(WRITE_FILE);
    for (auto memloc : addr) buffer_write_roption(memloc, buffer[memloc]);
    write_result(0, 100, READ_FILE);
}
// 전체 주소에 같은 정보 저장
void full_write(char data[10]) {
    for (unsigned short memloc = 0; memloc < 100; memloc++)
        buffer_write_roption(memloc, string(data));
    write_ssd(WRITE_FILE);
    init_buffer();
}
// roption 정보 최신화
void buffer_write_roption(unsigned short memloc, string data) {
    for (unsigned short j = 0; j < 4; j++)
        roption[memloc * 4 + j] = mkhex(&data[(j + 1) * 2]);
}
// buffer에 담긴 정보 nand에 최신화
void buffer_write_ssd() {
    read_ssd(WRITE_FILE);
    for (unsigned short memloc : addr)
        buffer_write_roption(memloc, buffer[memloc]);
    write_ssd(WRITE_FILE);
    init_buffer();
}
// write 명령 하달시 buffer에 저장, buffer 10개가 될 경우 nand에 저장
void stack_buffer_write(unsigned short memloc, string data) {
    buf_size++;
    if (addr.find(memloc) == addr.end()) addr.insert(memloc);
    buffer[memloc] = data;
    if (buf_size >= 10) buffer_write_ssd();
    else write_buffer();
}
// erase 명령 하달시 buffer에 저장, buffer 10개가 될 경우 nand에 저장
void stack_buffer_erase(unsigned short memloc, unsigned char memlen) {
    buf_size++;
    string erase_data = "0x00000000";
    for (unsigned char i = 0; i < memlen; i++) {
        if (addr.find(memloc + i) == addr.end()) addr.insert(memloc + i);
        buffer[memloc + i] = erase_data;
    }
    if (buf_size >= 10) buffer_write_ssd();
    else write_buffer();
}
// 프로그램 시작 후 buffer 정보 읽어와 buffer 정보 최신화
unsigned char read_buffer() {
    unsigned char cnt = 0;
    char buf[11];
    ifstream ifs(BUFFER);
    unsigned short loc;
    ifs.getline(buf, 10);
    cnt = buf[0] - '0';
    while (!ifs.eof()) {
        ifs.getline(buf, 11);
        if (buf[0] > '9' || buf[0] < '0') break;
        loc = (buf[0] - '0') * 10 + buf[1] - '0';
        addr.insert(loc);
        buffer[loc] = string(buf);
    }
    ifs.close();
    return cnt;
}
// buffer 정보를 buffer.txt에 최신화
void write_buffer() {
    ofstream ofs(BUFFER);
    ofs << (char)('0' + buf_size) << endl;
    int cnt = 0;
    for (auto memloc : addr) {
        ofs << (char)(memloc / 10 + '0') << (char(memloc % 10 + '0')) << &buffer[memloc][2] << endl;
    }
    ofs.close();
}
// shell 최초 실행 시 buffer 비워줌
void init_buffer() {
    ofstream ofs(BUFFER);
    ofs << '0';
    ofs.close();
}

int main(int argv, char* argc[]) {
    buf_size = read_buffer();

    switch (argc[1][0]) {

    case 'W':
        stack_buffer_write((unsigned short)stoi(argc[2]), argc[3]);
        break;
    case 'R':
        read((unsigned short)stoi(argc[2]));
        break;
    case 'F':
        if (argc[1][1] == 'R') full_read();
        else if (argc[1][1] == 'W') full_write(argc[2]);
        else if (argc[1][1] == '\0') buffer_write_ssd();
        break;
    case 'E':
        stack_buffer_erase((unsigned short)stoi(argc[2]), (unsigned char)stoi(argc[3]));
        break;
    case 'I':
        init_buffer();
        break;
    default:
        break;
    }
}