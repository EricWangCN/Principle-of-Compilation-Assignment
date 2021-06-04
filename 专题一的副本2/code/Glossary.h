//
// Created by 王子龙 on 2021/6/1.
//

#ifndef CODE_GLOSSARY_H
#define CODE_GLOSSARY_H
#include <string>
using namespace std;
typedef struct glossary
{
    std::string key;
    int seq;
    std::string mnemonic;
} Glossary;
Glossary GLO[100];
Glossary WSep[100];
Glossary DSep[100];

#endif //CODE_GLOSSARY_H
