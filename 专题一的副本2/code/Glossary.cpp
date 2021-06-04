//
// Created by 王子龙 on 2021/6/1.
//
#include <bits/stdc++.h>
typedef struct glossary
{
    std::string key;
    int seq;
    std::string mnemonic;
} Glossary;

using namespace std;
Glossary GLO[100] = {
        {"begin", 1, "BEGIN"},
        {"end", 2, "END"},
        {"main", 3, "MAIN"},
        {"if", 4, "IF"},
        {"then", 5, "THEN"},
        {"else", 6, "ELSE"},
        {"for", 7, "FOR"},
        {"do", 8, "DO"},
        {"while", 9, "WHILE"},
        {"identifier", 10, "ID"},
        {"static", 11, "STATIC"},
        {"int", 12, "INT"},
        {"void", 13, "VOID"},
        {"float", 14, "FLOAT"},
        {"double", 15, "DOUBLE"},
        {"end", 16, "END"},
        {"char", 17, "CHAR"},
        {"continue", 18, "CT"},
        {"long", 19, "LONG"},
        {"break", 20, "BREAK"},
        {"typedef", 21, "TD"},
        {"return", 22, "RETURN"},
        {"const", 23, "CONST"},
        {"switch", 24, "SWITCH"},
        {"case", 25, "CASE"}
};
Glossary WSep[100] = {
        {"+", 26, "ADD"},
        {"-", 27, "SUB"},
        {"*", 28, "MUL"},
        {"/", 29, "DIV"},
        {";", 30, "SEMI"},
        {",", 31, "COMMA"},
        {"(", 32, "LB"},
        {")", 33, "RB"},
        {"{", 34, "LBB"},
        {"}", 35, "RBB"},
        {"<", 36, "LT"},
        {"=", 37, "EQ"},
        {">", 38, "GT"},
        {"!", 39, "EXC"},
        {"&", 40, "POSA"},
        {"|", 41, "POSB"}
};
int a[] = {6,6,6};

Glossary DSep[100] = {
        {"<=", 42, "LE"},
        {"<<", 43, "LL"},
        {"==", 44, "EE"},
        {"<>", 45, "NEA"},
        {">=", 46, "GE"},
        {">>", 47, "GG"},
        {"++", 48, "ADDS"},
        {"+=", 49, "ADDE"},
        {"--", 50, "SUBS"},
        {"-=", 51, "SUBE"},
        {"*=", 52, "MULE"},
        {"/=", 53, "DIVE"},
        {"||", 54, "OR"},
        {"!=", 55, "NEB"},
        {"&&", 56, "AND"},
        {"//", 57, "NOTE"},
        {"/*", 58, "NOTF"},
        {"*/", 59, "NOTB"}
};
