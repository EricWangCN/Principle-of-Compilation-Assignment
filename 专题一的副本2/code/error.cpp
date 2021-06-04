//
// Created by 王子龙 on 2021/6/1.
//
#include <iostream>

void error(int type)
{
    type == 1?std::cout << "identifier exceeds maximum number" << std::endl:std::cout << "the initial of identifier can't be digit" << std::endl;
}