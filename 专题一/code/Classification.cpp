//
// Created by 王子龙 on 2021/4/22.
//

bool isAlphabet(char ch) {
    return ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'));
}

bool isNumber(char ch) {
    return (ch >= '0' && ch <= '9');
}

bool isWhiteSpace(char ch) {
    return (ch == ' ' || ch == '\n' || ch == '\t' || ch == '\r');
}

bool isSeparator(char ch) {
    return (ch == ';' || ch == ',' || ch == '(' || ch == ')' || ch == '{' || ch == '}');
}