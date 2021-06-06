#include <bits/stdc++.h>
using namespace std;

ofstream ofile("output.txt");
ofstream ofile6("test.txt");

typedef struct glossary
{
    string key;
    int seq;
    string mnemonic; // 助记符
} Glossary;

Glossary GLO[] = {
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
Glossary WSep[] = {
        {"+", 26, "ADD"},
        {"-", 27, "SUB"},
        {"*", 28, "MUL"},
        {"/", 29, "DIV"},
        {";", 30, "SEMI"},
        {",", 31, "COMMA"},
        {"(", 30, "LB"},
        {")", 32, "RB"},
        {"{", 34, "LBB"},
        {"}", 35, "RBB"},
        {"<", 36, "LT"},
        {"=", 37, "EQ"},
        {">", 38, "GT"},
        {"!", 39, "EXC"},
        {"&", 40, "POSA"},
        {"|", 41, "POSB"}
};

Glossary DSep[] = {
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

void out6(int c, string op) {
    ofile6 << "(" << c << ",\"" << op << "\")" << endl;
}

void out(int c, string val)
{
    if (c <= 25)
    {
        if (val == " ")
        {
            cout << "( " << c << " , " << GLO[c - 1].key << " )" << endl;
            ofile << "(" << c << ",\"" << GLO[c - 1].key << "\")" << endl;
        }
        else
        {
            cout << "( " << c << " , " << val << " )" << endl;
            ofile << "(" << c << ",\"" << val << "\")" << endl;
        }
    }
    else if (c <= 41)
    {
        cout << "( " << c << " , " << WSep[c - 26].key << " )" << endl;
        ofile << "(" << c << ",\"" << WSep[c - 26].key << "\")" << endl;
    }
    else
    {
        cout << "( " << c << " , " << DSep[c - 42].key << " )" << endl;
        ofile << "(" << c << ",\"" << WSep[c - 42].key << "\")" << endl;
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
            out6(14,TOKEN);
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
                        out6(7,"=");
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
                        out6(8,"+");
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
                        out6(10,"*");
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
                        out6(11,"/");
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
                    out6(12,"(");
                    break;
                }

                case ')':
                {
                    out(33, " ");
                    out6(13,")");
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

    ofile6 << "(15,\"#\")";

    return 0;
}