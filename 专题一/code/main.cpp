#include <bits/stdc++.h>
#include "Classification.h"
#include "Handler.h"
#include "Output.h"

using namespace std;

ifstream ifile("sample1.c");
ofstream ofile("output.txt");

int main() {
    char ch;  // 存放当前字符
    ifile >> noskipws;  // 允许读空格

    if (!ifile.is_open()) {
        cout << "Failed to open file." << endl;
        return 0;
    }
    while (!ifile.eof()) {
        ifile >> ch;
        string token(1, ch);  // 将当前字符装入字符串

        switch (ch) {

            // 纯单字符分界符
            case '{': case '}': case '(': case ')': case ',': case ';':
                output(token);
                break;

                // 单、双字符分解符 + - < >
            case '+': case '-': case '<': case '>':
                ifile >> ch;
                if (isWhiteSpace(ch)) {
                    output(token);
                }
                else if (ch == '=' || ch == token[0] || (token[0] == '<' && ch == '>')) {
                    token = token.append(1, ch);
                    handleEndOfWord(token);
                }
                else {
                    handleError();
                }
                break;

                // 单、双字符分解符 * / ! =
            case '*': case '/': case '!': case '=':
                ifile >> ch;
                if (isWhiteSpace(ch)) {
                    output(token);
                }
                else if (token[0] == '/' && ch == '/') {
                    char temp[255];
                    ifile.getline(temp, 255);
                }
                else if (token[0] == '/' && ch == '*') {
                    bool isWellEnded = false;  // 注释是否正确结束
                    while (!ifile.eof()) {
                        ifile >> ch;
                        if (ch == '*') {
                            ifile >> ch;
                            if (ch == '/') {
                                isWellEnded = true;
                                break;
                            }
                            else {
                                ifile.seekg(-1, ios::cur);
                            }
                        }
                    }
                    if (!isWellEnded) output("error");
                }
                else if (ch == '=') {
                    token = token.append(1, ch);
                    handleEndOfWord(token);
                }
                else {
                    handleError();
                }
                break;

                // 单、双字符分解符 & |
            case '&': case '|':
                ifile >> ch;
                if (isWhiteSpace(ch)) {
                    output(token);
                }
                else if (ch == token[0]) {
                    token = token.append(1, ch);
                    handleEndOfWord(token);
                }
                else {
                    handleError();
                }
                break;

                // 空
                case ' ': case '\n': case '\t': case '\r':
                break;

            default:

                // 整数
                if (isNumber(ch)) {
                    token = "";
                    do {
                        token += ch;
                        ifile >> ch;
                    } while (isNumber(ch));
                    // 正确结束
                    if (isWhiteSpace(ch) || isSeparator(ch)) {
                        output("integer", token);
                    }
                        // 错误结束
                    else {
                        do {
                            ifile >> ch;
                        } while (!isWhiteSpace(ch));
                        output("error");
                    };
                    ifile.seekg(-1, ios::cur);
                }

                    // 标识符
                else if (isAlphabet(ch)) {
                    token = "";
                    int length = 0;
                    do {
                        token += ch;
                        length++;
                        ifile >> ch;
                    } while (isAlphabet(ch) || isNumber(ch));
                    // 正确结束
                    if ((isWhiteSpace(ch) || isSeparator(ch)) && length <= 32) {
                        output("identifier", token);
                    }
                        // 错误结束
                    else {
                        do {
                            ifile >> ch;
                        } while (!isWhiteSpace(ch));
                        output("error");
                    };
                    ifile.seekg(-1, ios::cur);
                }

                    // 啥都不是
                else {
                    output("error");
                }
        }
    }
    ifile.close();
    ofile.close();

    return 0;
}
