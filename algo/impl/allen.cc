#include "../allen.h"

namespace allen {

//����
void swap(int a, int b) {
	if (a != b) { // ��ͬ�������Ϊ0
		a ^= b;//a = a^b;
		b ^= a;//b = b^(a^b) ==> b^b^a ==> ��ͬ�������Ϊ0 ==�� 0^a ==�� �κ�����0��򲻱� ==�� a
		a ^= b;//a = (a^b)^a ==> ͬ�Ͽ�֪ ==�� 0^b ==> b
	}
}

//�任����
int flip(int a) {
	return ~a + 1;
}

//����ֵ: ��0��򱣳ֲ��䣬��-1(0xFFFFFFFF)������ȡ����
int ab(int a) {
	int s = a >> 31;
	//return s == 0 ? a : (~a+1);   //ʹ���б�ʽ�汾

	//������s == 0   ==>   a^0 == a   ==>   a-0 == a  ����
	//������s == 0xFFFFFFFF   ==>   a^0xFFFFFFFF����ͬ��ȡ����  ==>   ��0���1
	//ex: -1 ==>  s == 0xFFFFFFFF  ==>  -1^-1 == 0 ==> 0-(-1) == 1
	//ex: -2 ==>  s == 0xFFFFFFFF  ==>  ԭ��(0000 0010),ȡ��(1111 1101),+1(1111 1110)
	//1111 1110 ȡ�� ==�� 0000 0001 +1 ==�� 0000 0010 ��Ϊ2
	return (a^s) - s;//�������б�ʽ�汾
}

//�ж��Ƿ�Ϊ����
bool odd(int a) {
	return a & 1;//a&1 == 1
}

char *my_strcpy(char *src, char *dst) {
	if (NULL == src || NULL == dst) return NULL;
	if (src == dst) return src;

	char *begin = dst;
	int src_len = strlen(src);
	if ((dst - src) < src_len) {//�ڴ��ַ�ص�,�Ӻ���ǰ����
		for (int i = src_len - 1; i >= 0; --i) {
			*(dst + i) = *(src + i);
		}
	}
	else {//��ǰ���󿽱�
		while ((*dst++ = *src++) != '\0');
	}
	return begin;
}

}