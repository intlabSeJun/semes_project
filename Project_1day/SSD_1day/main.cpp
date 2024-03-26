#include<iostream>
#include<fstream>
#include<string>

#define WRITE_FILE "nand.txt"
#define READ_FILE "result.txt"
#define BLOCK_SIZE 10
#define MAX_ADDR 100
#define MAX_ST 400

using namespace std;

unsigned char mkh(char data);
unsigned char mkhex(char data[2]);
char mkc_u(unsigned char data);
char mkc_d(unsigned char data);
void write(unsigned short memloc, char data[10]);
unsigned short mem_addr(string mem);
void write_ssd(unsigned short memloc, char data[10], unsigned char memlen, string filename);
void read_ssd(string filename);
void read(unsigned short memloc);
void full_read();
void full_write(char data[10]);
void write_result(unsigned short memloc, unsigned char memlen, string filename);

char roption[MAX_ST];

unsigned short mem_addr(string mem) {
    unsigned short memloc = 0;
    for (unsigned char i = 0; i < mem.length(); i++) {
        memloc *= 10;
        memloc += mem[i] - '0';
    }
    return memloc;
}

unsigned char mkh(char data) {
    if (data <= 'F' && data >= 'A') return data - 'A' + 10;
    else return data - '0';
}

unsigned char mkhex(char data[2]) {
    unsigned char ret = (mkh(data[0]) << 4) + mkh(data[1]);
    return ret;
}

char mkc_u(unsigned char data) {
    char ret;
    data /= 16;

    if (data >= 10) ret = 'A' + data - 10;
    else ret = '0' + data;

    return ret;
}

char mkc_d(unsigned char data) {
    char ret;
    data %= 16;

    if (data >= 10) ret = 'A' + data - 10;
    else ret = '0' + data;

    return ret;
}

void read_ssd(string filename) {
    ifstream ifs(filename);
    ifs.read(roption, 400);
    ifs.close();
}

void write_ssd(unsigned short memloc, char data[10], unsigned char memlen, string filename) {

    for (unsigned char i = 0; i < memlen; i++) {
        for (unsigned short j = 0; j < 4; j++) {
            roption[(memloc + i) * 4 + j] = mkhex(&data[(j + 1) * 2]);
        }
    }
    ofstream ofs(filename);
    ofs.write(roption, 400);
    ofs.close();
}

void write(unsigned short memloc, char data[10]) { // ok
    read_ssd(WRITE_FILE);
    write_ssd(memloc, data, 1, WRITE_FILE);
}

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

void read(unsigned short memloc) { // ok

    ifstream ifs(WRITE_FILE);
    ifs.seekg(memloc * 4, ios::beg);
    ifs.read(&roption[memloc * 4], 4);
    ifs.close();

    write_result(memloc, 1, READ_FILE);

}

void full_read() { // ok
    read_ssd(WRITE_FILE);
    write_result(0, 100, READ_FILE);

}

void full_write(char data[10]) { // ok
    write_ssd(0, data, 100, WRITE_FILE);
}

void erase(unsigned short memloc, unsigned char memlen) {
    char erase_data[10] = { '0', 'x', '0', '0', '0', '0', '0', '0', '0', '0' };
    read_ssd(WRITE_FILE);
    write_ssd(memloc, erase_data, memlen, WRITE_FILE);
}

int main(int argv, char* argc[]) {

    switch (argc[1][0]) {

    case 'W':
        write((unsigned short)stoi(argc[2]), argc[3]);
        break;
    case 'R':
        read((unsigned short)stoi(argc[2]));
        break;
    case 'F':
        if (argc[1][1] == 'R') full_read();
        else if (argc[1][1] == 'W') full_write(argc[2]);
        break;
    case 'E':
        erase((unsigned short)stoi(argc[2]), (unsigned char)stoi(argc[3]));
    default:
        break;
    }

}