#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <ctime>
using namespace std;

string intToString(int n) {
    string s = "315010";
    int bit_count = 0;
    int t = n;
    while (t > 0) {
        bit_count++;
        t /= 10;
    }
    while (bit_count < 4) {
        s += "0";
        bit_count++;
    }

    stringstream ss;
    ss << n;
    s += ss.str();
    return s;
}

int main() {
    char characters[26] = {'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z'};
    ofstream ofs("/Users/yusen/Documents/Github/miniSQL/test_file");
    srand((unsigned)time(NULL));
    for (int i = 1;i < 1000;i++) {
        ofs << "insert into test values(";
        string s = intToString(i);
        ofs << "'" << s << "',";
        ofs << "'" << characters[rand() % 26] << characters[rand() % 26] << characters[rand() % 26] << "',";
        ofs << rand() % 8 + 18 << ",";
        ofs << rand() % 5 + (rand() % 6) * 0.2 << ");" << endl;
    }
}