#pragma once

#include "../util/util.h"

namespace allen{
    
//交换
void swap(int a, int b);

//变换符号
int flip(int a);

//绝对值: 与0异或保持不变，与-1(0xFFFFFFFF)异或等于取反，
int ab(int a);

//判断是否为奇数
bool odd(int a);

char *my_strcpy(char *src, char *dst);

}