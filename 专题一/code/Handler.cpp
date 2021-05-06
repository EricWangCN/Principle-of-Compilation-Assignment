//
// Created by 王子龙 on 2021/4/22.
//
#include <fstream>
#include <string>
#include "Classification.h"
#include "Output.h"

using namespace std;
extern ifstream ifile;
extern ofstream ofile;

// 错误处理 读到不是空格的地方
void handleError() {
    char ch;
    do {
        ifile >> ch;
    } while (!isWhiteSpace(ch));
    output("error");
}

// 正确结束则打印 错误结束则错误处理
void handleEndOfWord(string str) {
    char ch;
    ifile >> ch;
    // 正确结束
    if (isWhiteSpace(ch) || isSeparator(ch)){
        output(str);
    }
        // 错误结束
    else {
        handleError();
    };
    ifile.seekg(-1, ios::cur);  // 文件指针回退
}