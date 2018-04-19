#include "../allen.h"

namespace allen {

//交换
void swap(int a, int b) {
	if (a != b) { // 相同的数异或为0
		a ^= b;//a = a^b;
		b ^= a;//b = b^(a^b) ==> b^b^a ==> 相同的数异或为0 ==》 0^a ==》 任何数与0异或不变 ==》 a
		a ^= b;//a = (a^b)^a ==> 同上可知 ==》 0^b ==> b
	}
}

//变换符号
int flip(int a) {
	return ~a + 1;
}

//绝对值: 与0异或保持不变，与-1(0xFFFFFFFF)异或等于取反，
int ab(int a) {
	int s = a >> 31;
	//return s == 0 ? a : (~a+1);   //使用判别式版本

	//正数：s == 0   ==>   a^0 == a   ==>   a-0 == a  不变
	//负数：s == 0xFFFFFFFF   ==>   a^0xFFFFFFFF（等同于取反）  ==>   加0或加1
	//ex: -1 ==>  s == 0xFFFFFFFF  ==>  -1^-1 == 0 ==> 0-(-1) == 1
	//ex: -2 ==>  s == 0xFFFFFFFF  ==>  原码(0000 0010),取反(1111 1101),+1(1111 1110)
	//1111 1110 取反 ==》 0000 0001 +1 ==》 0000 0010 即为2
	return (a^s) - s;//不适用判别式版本
}

//判断是否为奇数
bool odd(int a) {
	return a & 1;//a&1 == 1
}

char *my_strcpy(char *src, char *dst) {
	if (NULL == src || NULL == dst) return NULL;
	if (src == dst) return src;

	char *begin = dst;
	int src_len = strlen(src);
	if ((dst - src) < src_len) {//内存地址重叠,从后往前拷贝
		for (int i = src_len - 1; i >= 0; --i) {
			*(dst + i) = *(src + i);
		}
	}
	else {//从前往后拷贝
		while ((*dst++ = *src++) != '\0');
	}
	return begin;
}

}