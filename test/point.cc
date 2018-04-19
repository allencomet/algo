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

		int(*p)[4] = a;//p是一个指针，它指向一个数组，数组的类型为int[4]，这正是a所包含的每个一维数组的类型
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
		//数组指针与指针数组书写区别在于括号的作用域
		int(*p)[4] = a;//p是一个指针，它指向一个数组，数组的类型为int[4]，这正是a所包含的每个一维数组的类型
		int *(arr[4]);//指针数组,等同于int *arr[2];//是一个数组，每个元素保存的都是指针

		EXPECT_EQ(sizeof(void*), 8);//64bits 
		EXPECT_EQ(sizeof(a) / sizeof(int), 12);
		EXPECT_EQ(sizeof(p), sizeof(sizeof(a[1])));
		EXPECT_EQ(sizeof(p), 8);
		EXPECT_EQ(sizeof(*p), 16);
		EXPECT_EQ(sizeof(a[2][1]), sizeof(p[2][1]));
		EXPECT_EQ(sizeof(p[2][1]), 4);


		//对于指针数组的测试
		EXPECT_EQ(sizeof(arr), 4 * 8);//表示arr是存放4个指针的数组


		//dataType *arrName[len];//[]优先级高于*，所以该定义形式可以理解为
		//dataType *(arrName[len]);

		int x = 1, y = 2, z = 3;
		int *arrp[3] = { &x,&y,&z };

		//定义一个指向数组的指针
		int **pp = arrp;//pp 是指向数组 arrp 的指针，确切地说是指向 arrp 第 0 个元素的指针，它的定义形式应该理解为int *(*pp)
		EXPECT_EQ(*arrp[0], **(pp + 0));
		EXPECT_EQ(*arrp[1], **(pp + 1));
		EXPECT_EQ(*arrp[2], **(pp + 2));

		//字符串数组str中存放的并不是字符串，而是每个字符串的地址，字符串本身位于其他的内存区域，和字符串数组是分开的
		//也只有指针数组中每个元素的类型是char *时，才能像下面那样给指针数组赋值
		const char *str[3] = { "allen","comet","cpp" };
		EXPECT_EQ(str[0], "allen");
		EXPECT_EQ(str[1], "comet");
		EXPECT_EQ(str[2], "cpp");

		//下面的赋值方式与上面是等价的
		const char *str0 = "allen";
		const char *str1 = "comet";
		const char *str2 = "cpp";

		const char *straux[3] = { str0, str1, str2 };

		EXPECT_EQ(straux[0], "allen");
		EXPECT_EQ(straux[1], "comet");
		EXPECT_EQ(straux[2], "cpp");
	}

	DEF_case(function_pointer) {
		//函数指针的定义形式：returnType (*pointerName)(param list);//()优先级高于*
		int(*pmax)(int, int) = max;
		EXPECT_EQ((*pmax)(2, 3), 3);//通过函数指针调用真正的函数


		//由于直接定义一个函数指针过于冗长：用typedef 对其进行简化
		typedef int(*funType)(int, int);
		funType fun = max;
		EXPECT_EQ(fun(2, 3), 3);


		//void * (*(*fp1)(int))[10];
		//fp1是一个函数指针，形式为：*(*fp1)(int)，返回值是一个指针，外层是void *[10],因此fp1返回一个指针，
		//该指针指向一个包含10个void*类型的数据的数组
		//void *returnType[10];//指针数组，上面(*(*fp1)(int))替换成returnType即表示这个函数指针返回一个指针数组


		//float (*(*fp2)(int, int, float))(int);
		//fp2是一个函数指针，形式为：*(*fp2)(int, int, float)，返回值是一个指针，外层是float returnType(int)
		//返回一个指针，该指针指向一个函数，即返回一个函数指针


		//typedef double (*(*(*fp3)())[10])();
		//fp3 a;
		//fp3是一个函数指针，形式为：*(*fp3)()，将其替换成type则为：double (*(type)[10])(); //而[]优先级高于*
		//返回的指针指向含有10个元素的函数指针数组，这10个元素的类型为函数指针double (*funptr)()

		//int (*(*fp4)[10])(); 
		//fp4是一个指针数组，这个指针数组是包含10个函数指针的数组，这些函数指针是int (*fun)()

		//(*(void(*)())0)();

	}
}

}//namespace test
