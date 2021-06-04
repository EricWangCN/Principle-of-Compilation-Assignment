//
// Created by 王子龙 on 2021/6/1.
//
#include <bits/stdc++.h>
#include "Glossary.h"
#include "Looks.h"
#include "error.h"
#include "out.h"
using namespace std;
extern char TOKEN[33];
int analysis(FILE *fp)
{

    char ch;
    int cur_t;
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