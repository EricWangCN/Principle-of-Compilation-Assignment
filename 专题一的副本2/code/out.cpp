//
// Created by 王子龙 on 2021/6/1.
//
#include "Glossary.h"
#include <iostream>
using namespace std;
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