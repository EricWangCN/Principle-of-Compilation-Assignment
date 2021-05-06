/*
 * 编译原理 实验1
 *
 * by 夏铭轩 18281166 BJTU CS1803
 * 
 * 2021.04.17
 */
#include<iostream>                       
#include<map>
#include<fstream>
using namespace std;

// 全局变量
ifstream ifile("sample1.c");
ofstream ofile("output.txt");
map<string, string> word2id = {
	{"identifier", "01"},
	{"integer", "02"},
	{"+", "03"},
	{"-", "04"},
	{"*", "05"},
	{"/", "06"},
	{"!", "07"},
	{">", "08"},
	{"<", "09"},
	{"&", "10"},
	{"|", "11"},
	{"=", "12"},
	{";", "13"},
	{",", "14"},
	{"(", "15"},
	{")", "16"},
	{"{", "17"},
	{"}", "18"},
	{">=", "19"},
	{"<=", "20"},
	{"<>", "21"},
	{"!=", "22"},
	{"==", "23"},
	{"++", "24"},
	{"--", "25"},
	{">>", "26"},
	{"<<", "27"},
	{"+=", "28"},
	{"-=", "29"},
	{"*=", "30"},
	{"/=", "31"},
	{"&&", "32"},
	{"||", "33"},
	{"void", "34"},
	{"int", "35"},
	{"float", "36"},
	{"double", "37"},
	{"if", "38"},
	{"else", "39"},
	{"for", "40"},
	{"do", "41"},
	{"while", "42"},
	{"return", "43"},
	{"main", "44"}
};

bool isAlphabet(char ch) {
	if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')) {
		return true;
	}
	else return false;
}

bool isNumber(char ch) {
	if (ch >= '0' && ch <= '9') {
		return true;
	}
	else return false;
}

bool isWhiteSpace(char ch) {
	if (ch == ' ' || ch == '\n' || ch == '\t') return true;
	return false;
}

bool isSeparator(char ch) {
	if (ch == ';' || ch == ',' || ch == '(' || ch == ')' || ch == '{' || ch == '}') return true;
	return false;
}

void output(string type, string item = "") {
	if (type == "error") {
		ofile << "error" << endl;
		cout << "error" << endl;
	}
	else if (type == "integer") {
		ofile << "[" + word2id[type] + ", " + item + "]" << endl;
		cout << "[" + word2id[type] + ", " + item + "]" << endl;
	}
	else if (type == "identifier") {
		// 判断是否为保留字
		if (word2id.count(item) == 1) {
			ofile << "[" + word2id[item] + ", " + item + "]" << endl;
			cout << "[" + word2id[item] + ", " + item + "]" << endl;
		}
		else {
			ofile << "[" + word2id[type] + ", " + item + "]" << endl;
			cout << "[" + word2id[type] + ", " + item + "]" << endl;
		}
	}
	else {
		ofile << "[" + word2id[type] + ", " + type + "]" << endl;
		cout << "[" + word2id[type] + ", " + type + "]" << endl;
	}
}

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

int main() {

	char ch;  // 存放当前字符
	ifile >> noskipws;  // 允许读空格

	cout << "【请保证输入文件以两个回车结尾】" << endl;

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
			case ' ': case '\n': case '\t':
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
