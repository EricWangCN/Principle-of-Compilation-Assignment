//
// Created by 王子龙 on 2021/4/22.
//
#include <bits/stdc++.h>

using namespace std;

extern ifstream ifile;
extern ofstream ofile;

std::map<std::string, std::string> IDofWords = {
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

void output(string type, string item = "") {
    if (type == "error") {
        ofile << "error" << endl;
        cout << "error" << endl;
    }
    else if (type == "integer") {
        ofile << "[" + IDofWords[type] + ", " + item + "]" << endl;
        cout << "[" + IDofWords[type] + ", " + item + "]" << endl;
    }
    else if (type == "identifier") {
        // 判断是否为保留字
        if (IDofWords.count(item) == 1) {
            ofile << "[" + IDofWords[item] + ", " + item + "]" << endl;
            cout << "[" + IDofWords[item] + ", " + item + "]" << endl;
        }
        else {
            ofile << "[" + IDofWords[type] + ", " + item + "]" << endl;
            cout << "[" + IDofWords[type] + ", " + item + "]" << endl;
        }
    }
    else {
        ofile << "[" + IDofWords[type] + ", " + type + "]" << endl;
        cout << "[" + IDofWords[type] + ", " + type + "]" << endl;
    }
}