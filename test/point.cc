#include "../util/util.h"

namespace test {

int max(int a, int b) {
	return a > b ? a : b;
}

DEF_test(pointer){
	int a[3][4] = { {1,2,3,4},{5,6,7,8},{9,10,11,12} };

	DEF_case(index) {
		EXPECT_EQ(a[2][1], *(*(a + 2) + 1));//a[2][1] == *(*(a+2)+1)
		EXPECT_EQ(a[2][1], (*(a + 2))[1]);//a[2][1] == (*(a+2))[1]

		int(*p)[4] = a;//p��һ��ָ�룬��ָ��һ�����飬���������Ϊint[4]��������a��������ÿ��һά���������
		EXPECT_EQ(a[2][1], *(*(p + 2) + 1));//a[2][1] == *(*(p+2)+1)

		for (int i = 0; i < 3; ++i) {
			for (int j = 0; j < 4; ++j) {
				EXPECT_EQ(a[i][j], *(*(a + i) + j));//a[i][j] == *(*(a+i)+j)
				EXPECT_EQ(a[i][j], (*(a + i))[j]);//a[i][j] == (*(a + i))[j]
				EXPECT_EQ(a[i][j], *(a[i]+j));//a[i][j] == *(a[i]+j)
				EXPECT_EQ(a[i][j], *(*(p + i) + j));//a[i][j] == *(*(p+i)+j)
			}
		}

		
	}

	DEF_case(size) {
		//����ָ����ָ��������д�����������ŵ�������
		int(*p)[4] = a;//p��һ��ָ�룬��ָ��һ�����飬���������Ϊint[4]��������a��������ÿ��һά���������
		int *(arr[4]);//ָ������,��ͬ��int *arr[2];//��һ�����飬ÿ��Ԫ�ر���Ķ���ָ��

		EXPECT_EQ(sizeof(void*), 8);//64bits 
		EXPECT_EQ(sizeof(a) / sizeof(int), 12);
		EXPECT_EQ(sizeof(p), sizeof(sizeof(a[1])));
		EXPECT_EQ(sizeof(p), 8);
		EXPECT_EQ(sizeof(*p), 16);
		EXPECT_EQ(sizeof(a[2][1]), sizeof(p[2][1]));
		EXPECT_EQ(sizeof(p[2][1]), 4);


		//����ָ������Ĳ���
		EXPECT_EQ(sizeof(arr), 4 * 8);//��ʾarr�Ǵ��4��ָ�������


		//dataType *arrName[len];//[]���ȼ�����*�����Ըö�����ʽ�������Ϊ
		//dataType *(arrName[len]);

		int x = 1, y = 2, z = 3;
		int *arrp[3] = { &x,&y,&z };

		//����һ��ָ�������ָ��
		int **pp = arrp;//pp ��ָ������ arrp ��ָ�룬ȷ�е�˵��ָ�� arrp �� 0 ��Ԫ�ص�ָ�룬���Ķ�����ʽӦ�����Ϊint *(*pp)
		EXPECT_EQ(*arrp[0], **(pp + 0));
		EXPECT_EQ(*arrp[1], **(pp + 1));
		EXPECT_EQ(*arrp[2], **(pp + 2));

		//�ַ�������str�д�ŵĲ������ַ���������ÿ���ַ����ĵ�ַ���ַ�������λ���������ڴ����򣬺��ַ��������Ƿֿ���
		//Ҳֻ��ָ��������ÿ��Ԫ�ص�������char *ʱ������������������ָ�����鸳ֵ
		const char *str[3] = { "allen","comet","cpp" };
		EXPECT_EQ(str[0], "allen");
		EXPECT_EQ(str[1], "comet");
		EXPECT_EQ(str[2], "cpp");

		//����ĸ�ֵ��ʽ�������ǵȼ۵�
		const char *str0 = "allen";
		const char *str1 = "comet";
		const char *str2 = "cpp";

		const char *straux[3] = { str0, str1, str2 };

		EXPECT_EQ(straux[0], "allen");
		EXPECT_EQ(straux[1], "comet");
		EXPECT_EQ(straux[2], "cpp");
	}

	DEF_case(function_pointer) {
		//����ָ��Ķ�����ʽ��returnType (*pointerName)(param list);//()���ȼ�����*
		int(*pmax)(int, int) = max;
		EXPECT_EQ((*pmax)(2, 3), 3);//ͨ������ָ����������ĺ���


		//����ֱ�Ӷ���һ������ָ������߳�����typedef ������м�
		typedef int(*funType)(int, int);
		funType fun = max;
		EXPECT_EQ(fun(2, 3), 3);


		//void * (*(*fp1)(int))[10];
		//fp1��һ������ָ�룬��ʽΪ��*(*fp1)(int)������ֵ��һ��ָ�룬�����void *[10],���fp1����һ��ָ�룬
		//��ָ��ָ��һ������10��void*���͵����ݵ�����
		//void *returnType[10];//ָ�����飬����(*(*fp1)(int))�滻��returnType����ʾ�������ָ�뷵��һ��ָ������


		//float (*(*fp2)(int, int, float))(int);
		//fp2��һ������ָ�룬��ʽΪ��*(*fp2)(int, int, float)������ֵ��һ��ָ�룬�����float returnType(int)
		//����һ��ָ�룬��ָ��ָ��һ��������������һ������ָ��


		//typedef double (*(*(*fp3)())[10])();
		//fp3 a;
		//fp3��һ������ָ�룬��ʽΪ��*(*fp3)()�������滻��type��Ϊ��double (*(type)[10])(); //��[]���ȼ�����*
		//���ص�ָ��ָ����10��Ԫ�صĺ���ָ�����飬��10��Ԫ�ص�����Ϊ����ָ��double (*funptr)()

		//int (*(*fp4)[10])(); 
		//fp4��һ��ָ�����飬���ָ�������ǰ���10������ָ������飬��Щ����ָ����int (*fun)()

		//(*(void(*)())0)();

	}
}

}//namespace test
