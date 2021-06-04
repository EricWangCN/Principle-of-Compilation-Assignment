#include <bits/stdc++.h>
#include "Glossary.h"
#include "Analysis.h"
using namespace std;

char TOKEN[33];

int main()
{
    FILE *fp;
    int flag = 1;
    fp = fopen("test.txt", "r+");

    while (flag != 0)
    {
        flag = analysis(fp);
    }

    return 0;
}