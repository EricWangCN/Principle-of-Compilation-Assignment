//
// Created by 王子龙 on 2021/6/1.
//
#include "Glossary.h"
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