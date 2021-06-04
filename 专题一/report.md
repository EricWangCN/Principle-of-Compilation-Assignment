# 编译原理实验报告
王子龙 18281218 wangzilong@bjtu.edu.cn
## 实验环境
类目 | 详情
:-: | ---
操作系统 | macOS Big Sur 11.3.1 
CPU | Intel Core i5-7260U@2.3Ghz x2
IDE | CLion 2021.1.1 Build #CL-211.7142.21
Compiler | Apple clang version 11.0.0 (clang-1100.0.33.8)
## 实验要求（实验功能描述）
### 实验项目
以下为正则文法所描述的C语言子集单词符号的示例，请补充单词符号： ++，--， >>,  <<, += , -= ,\*=, /= ，&&（逻辑与），||（逻辑或），！（逻辑非）等等，给出补充后描述C语言子集单词符号的正则文法，设计并实现其词法分析程序。
<标识符>→字母︱ <标识符>字母︱ <标识符>数字
<无符号整数>→数字︱ <无符号整数>数字
<单字符分界符> →+ ︱－ ︱\* ︱；︱, ︱(︱) ︱{︱}
<双字符分界符>→<大于>=︱<小于>=︱<小于>>︱<感叹号>=︱<等于>=︱<斜竖>\*    <小于>→<      <等于>→=      <大于>→>      <斜竖> →/
<感叹号>→!
该语言的保留字 ：void、int、float、double、if、else、for、do、while 等等（也可补充）。   

### 设计说明
- 可将该语言设计成大小写不敏感，也可设计成大小写敏感，用户定义的标识符最长不超过32个字符；
- 字母为a-z A-Z，数字为0-9；
- 可以对上述文法进行扩充和改造；（4）“/*……*/”和“//”(一行内)为程序的注释部分。
### 设计要求
- 给出各单词符号的类别编码；
- 词法分析程序应能发现输入串中的错误；
- 词法分析作为单独一遍编写，词法分析结果为二元式序列组成的中间文件；（4）设计两个测试用例（尽可能完备），并给出测试结果。
## 主要数据结构描述
### 字符表数据结构
``` C++
typedef struct glossary
{
    string key;
    int seq;
    string mnemonic;
} Glossary;
```
字符表每一个表项均为一个结构体，包含表示符号名称的`key`，类别编码的`seq`，以及助记符`mnemonic`。字符表的三个部分，保留字表，单字分隔符表，双字分隔符表均用结构体数组实现。
## 程序结构描述
### lookup	
`Lookup`函数用于在保留字字符表数组中寻找给定`Token`字符串，如果找到，返回对应保留字的结构体，如果没有则返回一个`key`，`mnemonic`均为空串，`seq`等于0的结构体。
```C++
Glossary lookup(string token)
{
    int len = sizeof(GLO) / sizeof(GLO[0]);
    for (int i = 0; i < len; i++)
    {
        if (token == GLO[i].key)
        {
            return GLO[i];
        }
    }
    Glossary res = {" ", 0, " "};
    return res;
}
```
### lookseq
`Lookseq`函数用于在单字分隔符中寻找给定字符`ch`，如果找到返回对应单字分隔符的结构体，如果没有则返回一个`key`，`mnemonic`均为空串，`seq`等于0的结构体。因为双字分隔符的首字符必是单字分隔符，所以通过此函数便可以判断一串字符后面是否有分隔符存在。
```C++
Glossary lookSep(char ch)
{
    char *sep = new char[2];
    sep[0] = ch;
    sep[1] = '\0';
    int len = sizeof(WSep) / sizeof(WSep[0]);
    for (int i = 0; i < len; i++)
    {
        if (sep == WSep[i].key)
        {
            return WSep[i];
        }
    }
    Glossary res = {" ", 0, " "};
    return res;
}
```
### out
`Out`函数用于输出二元组，`c`如果为0，则认定`Token`包含的是用户自定义变量， 如果不为`0`，则按照`c`对应的字符表表项输出。
```C++
void out(int c, string val)
{
    if (c <= 25)
    {
        if (val == " ")
        {
            cout << "( " << c << " , " << GLO[c - 1].key << " )" << endl;
        }
        else
        {
            cout << "( " << c << " , " << val << " )" << endl;
        }
        return;
    }
    else if (c <= 41)
    {
        cout << "( " << c << " , " << WSep[c - 26].key << " )" << endl;
    }
    else
    {
        cout << "( " << c << " , " << DSep[c - 42].key << " )" << endl;
    }
}
```
### Error
`Error`函数是用于进行错误提示。
```C++
void error(int type)
{
    switch (type)
    {
        case 1:
        {
            cout << "identifier exceeds maximum number" << endl;
            break;
        }
        case 2:
        {
            cout << "the initial of identifier can't be digit" << endl;
            break;
        }
        default:
            break;
    }
}
```
### Analysis
`Analysis`函数为词法分析主体函数，对文本文件中的源程序字符串进行词法分析。
```C++
int analysis(FILE *fp)
{

    char ch;
    int cur_t = 1, sequence;
    Glossary word;
    ch = fgetc(fp);
    while (ch == ' ')
    {
        ch = fgetc(fp);
    }
    if (isalpha(ch))
    {
        TOKEN[0] = ch;
        ch = fgetc(fp);
        cur_t = 1;
        while (isalnum(ch) && cur_t < 32)
        {
            TOKEN[cur_t] = ch;
            cur_t++;
            ch = fgetc(fp);
        }
        if (cur_t == 32 && ch != ' ' && lookSep(ch).seq == 0)
        {
            error(1);
        }
        TOKEN[cur_t] = '\0';
        fseek(fp, -1, 1);
        word = lookup(TOKEN);
        if (word.seq == 0)
        {
            out(9, TOKEN);
        }
        else
        {
            out(word.seq, " ");
            if (word.seq == 2)
            {
                return 0;
            }
        }
    }
    else
    {
        if (isdigit(ch))
        {
            TOKEN[0] = ch;
            ch = fgetc(fp);
            cur_t = 1;
            while (isdigit(ch) && cur_t < 32)
            {
                TOKEN[cur_t] = ch;
                ch = fgetc(fp);
                cur_t++;
            }
            if (ch != ' ')
            {
                if (lookSep(ch).seq == 0)
                {
                    error(2);
                }
            }
            TOKEN[cur_t] = '\0';
            fseek(fp, -1, 1);
            out(12, TOKEN);
        }
        else
        {
            switch (ch)
            {

                case '<':
                {
                    ch = fgetc(fp);
                    if (ch == '=')
                        out(42, " ");
                    else if (ch == '<')
                        out(36, " ");
                    else
                    {
                        fseek(fp, -1, 1);
                        out(26, " ");
                    }
                    break;
                }
                case '=':
                {
                    ch = fgetc(fp);
                    if (ch == '=')
                        out(44, " ");
                    else
                    {
                        fseek(fp, -1, 1);
                        out(37, " ");
                    }
                    break;
                }
                case '>':
                {
                    ch = fgetc(fp);
                    if (ch == '=')
                        out(46, " ");
                    else
                    {
                        fseek(fp, -1, 1);
                        out(38, " ");
                    }
                    break;
                }
                case '+':
                {
                    ch = fgetc(fp);
                    if (ch == '=')
                        out(49, " ");
                    else if (ch == '+')
                        out(48, " ");
                    else
                    {
                        fseek(fp, -1, 1);
                        out(26, " ");
                    }
                    break;
                }
                case '-':
                {
                    ch = fgetc(fp);
                    if (ch == '=')
                        out(51, " ");
                    else if (ch == '-')
                        out(50, " ");
                    else
                    {
                        fseek(fp, -1, 1);
                        out(27, " ");
                    }
                    break;
                }
                case '*':
                {
                    ch = fgetc(fp);
                    if (ch == '=')
                        out(52, " ");
                    else
                    {
                        fseek(fp, -1, 1);
                        out(28, " ");
                    }
                    break;
                }
                case '/':
                {
                    ch = fgetc(fp);
                    if (ch == '=')
                        out(53, " ");
                    else if (ch == '/')
                    {
                        out(57, " ");
                        while (ch != '\n')
                        {
                            ch = fgetc(fp);
                        }
                        break;
                    }
                    else if (ch == '*')
                    {
                        out(58, " ");
                        while (1)
                        {
                            ch = fgetc(fp);
                            while (ch != '*')
                            {
                                ch = fgetc(fp);
                            }
                            ch = fgetc(fp);
                            if (ch != '/')
                            {
                            }
                            else
                            {
                                out(59, " ");
                                break;
                            }
                        }
                        break;
                    }

                    else
                    {
                        fseek(fp, -1, 1);
                        out(29, " ");
                    }
                    break;
                }
                case '&':
                {
                    ch = fgetc(fp);
                    if (ch == '&')
                        out(56, " ");
                    else
                    {
                        fseek(fp, -1, 1);
                        out(40, " ");
                    }
                    break;
                }
                case '|':
                {
                    ch = fgetc(fp);
                    if (ch == '|')
                        out(54, " ");
                    else
                    {
                        fseek(fp, -1, 1);
                        out(41, " ");
                    }
                    break;
                }
                case '!':
                {
                    ch = fgetc(fp);
                    if (ch == '=')
                        out(55, " ");
                    else
                    {
                        fseek(fp, -1, 1);
                        out(39, " ");
                    }
                    break;
                }
                case ';':
                {
                    out(30, " ");
                    break;
                }

                case ',':
                {
                    out(31, " ");
                    break;
                }

                case '(':
                {
                    out(32, " ");
                    break;
                }

                case ')':
                {
                    out(33, " ");
                    break;
                }
                case '{':
                {
                    out(34, " ");
                    break;
                }
                case '}':
                {
                    out(35, " ");
                    break;
                }
                default:
                    break;
            }
        }
    }
    return 1;
}
```
## 程序测试
### 测试文件1：
```C++
begin
void main() {
    int sum = 0;
    for(int i = 0; i <= 10; i++) {
        sum += i << 2;
        sum += i >> 2;
        sum += 2;
        sum *= 2;
        sum /= 2;
        sum -= 2;
     if (sum == 63 && i < 5 || sum == 128)
          break;
     i!=3;
    }

    return 0;
}
end
```
运行结果
```
( 1 , begin )
( 13 , void )
( 3 , main )
( 32 , ( )
( 33 , ) )
( 34 , { )
( 12 , int )
( 9 , sum )
( 37 , = )
( 12 , 0 )
( 30 , ; )
( 7 , for )
( 32 , ( )
( 12 , int )
( 9 , i )
( 37 , = )
( 12 , 0 )
( 30 , ; )
( 9 , i )
( 42 , <= )
( 12 , 10 )
( 30 , ; )
( 9 , i )
( 48 , ++ )
( 33 , ) )
( 34 , { )
( 9 , sum )
( 49 , += )
( 9 , i )
( 36 , < )
( 12 , 2 )
( 30 , ; )
( 9 , sum )
( 49 , += )
( 9 , i )
( 38 , > )
( 38 , > )
( 12 , 2 )
( 30 , ; )
( 9 , sum )
( 49 , += )
( 12 , 2 )
( 30 , ; )
( 9 , sum )
( 52 , *= )
( 12 , 2 )
( 30 , ; )
( 9 , sum )
( 53 , /= )
( 12 , 2 )
( 30 , ; )
( 9 , sum )
( 51 , -= )
( 12 , 2 )
( 30 , ; )
( 4 , if )
( 32 , ( )
( 9 , sum )
( 44 , == )
( 12 , 63 )
( 56 , && )
( 9 , i )
( 26 , + )
( 12 , 5 )
( 54 , || )
( 9 , sum )
( 44 , == )
( 12 , 128 )
( 33 , ) )
( 20 , break )
( 30 , ; )
( 9 , i )
( 55 , != )
( 12 , 3 )
( 30 , ; )
( 35 , } )
( 22 , return )
( 12 , 0 )
( 30 , ; )
( 35 , } )
( 2 , end )
```
### 测试文件2：
```C++
begin
int main() {
    int a = 3;
    int b = 666;
    while (a > 0) {
        a--;
        b -= 3;
    }

    return 0;
}
end
```
运行结果：
```
( 1 , begin )
( 12 , int )
( 3 , main )
( 32 , ( )
( 33 , ) )
( 34 , { )
( 12 , int )
( 9 , a )
( 37 , = )
( 12 , 3 )
( 30 , ; )
( 12 , int )
( 9 , b )
( 37 , = )
( 12 , 666 )
( 30 , ; )
( 9 , while )
( 32 , ( )
( 9 , a )
( 38 , > )
( 12 , 0 )
( 33 , ) )
( 34 , { )
( 9 , a )
( 50 , -- )
( 30 , ; )
( 9 , b )
( 51 , -= )
( 12 , 3 )
( 30 , ; )
( 35 , } )
( 22 , return )
( 12 , 0 )
( 30 , ; )
( 35 , } )
( 2 , end )
```
## 附件
### main.cpp
```C++
#include <bits/stdc++.h>
using namespace std;

ofstream ofile("output.txt");

typedef struct glossary
{
    string key;
    int seq;
    string mnemonic;
} Glossary;

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

char TOKEN[33];
Glossary lookup(string token);
void out(int c, string val);
void error(int type);

Glossary lookSep(char ch)
{
    char *sep = new char[2];
    sep[0] = ch;
    sep[1] = '\0';
    int len = sizeof(WSep) / sizeof(WSep[0]);
    for (int i = 0; i < len; i++)
    {
        if (sep == WSep[i].key)
        {
            return WSep[i];
        }
    }
    Glossary res = {" ", 0, " "};
    return res;
}

Glossary lookup(string token)
{
    int len = sizeof(GLO) / sizeof(GLO[0]);
    for (int i = 0; i < len; i++)
    {
        if (token == GLO[i].key)
        {
            return GLO[i];
        }
    }
    Glossary res = {" ", 0, " "};
    return res;
}

void out(int c, string val)
{
    if (c <= 25)
    {
        if (val == " ")
        {
            cout << "( " << c << " , " << GLO[c - 1].key << " )" << endl;
            ofile << "( " << c << " , " << GLO[c - 1].key << " )" << endl;
        }
        else
        {
            cout << "( " << c << " , " << val << " )" << endl;
            ofile << "( " << c << " , " << val << " )" << endl;
        }
    }
    else if (c <= 41)
    {
        cout << "( " << c << " , " << WSep[c - 26].key << " )" << endl;
        ofile << "( " << c << " , " << WSep[c - 26].key << " )" << endl;
    }
    else
    {
        cout << "( " << c << " , " << DSep[c - 42].key << " )" << endl;
        ofile << "( " << c << " , " << DSep[c - 42].key << " )" << endl;
    }
}

void error(int type)
{
    switch (type)
    {
        case 1:
        {
            cout << "identifier exceeds maximum number" << endl;
            break;
        }
        case 2:
        {
            cout << "the initial of identifier can't be digit" << endl;
            break;
        }
        default:
            break;
    }
}

int analysis(FILE *fp)
{

    char ch;
    int cur_t = 1, sequence;
    Glossary word;
    ch = fgetc(fp);
    while (ch == ' ')
    {
        ch = fgetc(fp);
    }
    if (isalpha(ch))
    {
        TOKEN[0] = ch;
        ch = fgetc(fp);
        cur_t = 1;
        while (isalnum(ch) && cur_t < 32)
        {
            TOKEN[cur_t] = ch;
            cur_t++;
            ch = fgetc(fp);
        }
        if (cur_t == 32 && ch != ' ' && lookSep(ch).seq == 0)
        {
            error(1);
        }
        TOKEN[cur_t] = '\0';
        fseek(fp, -1, 1);
        word = lookup(TOKEN);
        if (word.seq == 0)
        {
            out(9, TOKEN);
        }
        else
        {
            out(word.seq, " ");
            if (word.seq == 2)
            {
                return 0;
            }
        }
    }
    else
    {
        if (isdigit(ch))
        {
            TOKEN[0] = ch;
            ch = fgetc(fp);
            cur_t = 1;
            while (isdigit(ch) && cur_t < 32)
            {
                TOKEN[cur_t] = ch;
                ch = fgetc(fp);
                cur_t++;
            }
            if (ch != ' ')
            {
                if (lookSep(ch).seq == 0)
                {
                    error(2);
                }
            }
            TOKEN[cur_t] = '\0';
            fseek(fp, -1, 1);
            out(12, TOKEN);
        }
        else
        {
            switch (ch)
            {

                case '<':
                {
                    ch = fgetc(fp);
                    if (ch == '=')
                        out(42, " ");
                    else if (ch == '<')
                        out(36, " ");
                    else
                    {
                        fseek(fp, -1, 1);
                        out(26, " ");
                    }
                    break;
                }
                case '=':
                {
                    ch = fgetc(fp);
                    if (ch == '=')
                        out(44, " ");
                    else
                    {
                        fseek(fp, -1, 1);
                        out(37, " ");
                    }
                    break;
                }
                case '>':
                {
                    ch = fgetc(fp);
                    if (ch == '=')
                        out(46, " ");
                    else
                    {
                        fseek(fp, -1, 1);
                        out(38, " ");
                    }
                    break;
                }
                case '+':
                {
                    ch = fgetc(fp);
                    if (ch == '=')
                        out(49, " ");
                    else if (ch == '+')
                        out(48, " ");
                    else
                    {
                        fseek(fp, -1, 1);
                        out(26, " ");
                    }
                    break;
                }
                case '-':
                {
                    ch = fgetc(fp);
                    if (ch == '=')
                        out(51, " ");
                    else if (ch == '-')
                        out(50, " ");
                    else
                    {
                        fseek(fp, -1, 1);
                        out(27, " ");
                    }
                    break;
                }
                case '*':
                {
                    ch = fgetc(fp);
                    if (ch == '=')
                        out(52, " ");
                    else
                    {
                        fseek(fp, -1, 1);
                        out(28, " ");
                    }
                    break;
                }
                case '/':
                {
                    ch = fgetc(fp);
                    if (ch == '=')
                        out(53, " ");
                    else if (ch == '/')
                    {
                        out(57, " ");
                        while (ch != '\n')
                        {
                            ch = fgetc(fp);
                        }
                        break;
                    }
                    else if (ch == '*')
                    {
                        out(58, " ");
                        while (1)
                        {
                            ch = fgetc(fp);
                            while (ch != '*')
                            {
                                ch = fgetc(fp);
                            }
                            ch = fgetc(fp);
                            if (ch != '/')
                            {
                            }
                            else
                            {
                                out(59, " ");
                                break;
                            }
                        }
                        break;
                    }

                    else
                    {
                        fseek(fp, -1, 1);
                        out(29, " ");
                    }
                    break;
                }
                case '&':
                {
                    ch = fgetc(fp);
                    if (ch == '&')
                        out(56, " ");
                    else
                    {
                        fseek(fp, -1, 1);
                        out(40, " ");
                    }
                    break;
                }
                case '|':
                {
                    ch = fgetc(fp);
                    if (ch == '|')
                        out(54, " ");
                    else
                    {
                        fseek(fp, -1, 1);
                        out(41, " ");
                    }
                    break;
                }
                case '!':
                {
                    ch = fgetc(fp);
                    if (ch == '=')
                        out(55, " ");
                    else
                    {
                        fseek(fp, -1, 1);
                        out(39, " ");
                    }
                    break;
                }
                case ';':
                {
                    out(30, " ");
                    break;
                }

                case ',':
                {
                    out(31, " ");
                    break;
                }

                case '(':
                {
                    out(32, " ");
                    break;
                }

                case ')':
                {
                    out(33, " ");
                    break;
                }
                case '{':
                {
                    out(34, " ");
                    break;
                }
                case '}':
                {
                    out(35, " ");
                    break;
                }
                default:
                    break;
            }
        }
    }
    return 1;
}

int main()
{
    FILE *fp;
    int flag = 1;
    fp = fopen("test.c", "r+");

    while (flag != 0)
        flag = analysis(fp);

    return 0;
}
```
### 样例1
```C++
begin
void main() {
    int sum = 0;
    for(int i = 0; i <= 10; i++) {
        sum += i << 2;
        sum += i >> 2;
        sum += 2;
        sum *= 2;
        sum /= 2;
        sum -= 2;
     if (sum == 63 && i < 5 || sum == 128)
          break;
     i!=3;
    }

    return 0;
}
end
```
### 样例2
```C++
begin
int main() {
    int a = 3;
    int b = 666;
    while (a > 0) {
        a--;
        b -= 3;
    }

    return 0;
}
end
```