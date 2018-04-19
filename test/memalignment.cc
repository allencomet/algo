#include "../util/util.h"

#include "../design/memalignment.h"

namespace test {

////编译期间会有警告
//char *my_return_value1() {
//	char *str = "ABCD";//其实会强转成const char *
//	return str;
//}
//
////编译期间会有警告
//char *my_return_value2() {
//	static char *str = "ABCD";
//	return str;
//}
//
////编译期间会有警告
//char *my_return_value3() {
//	char str[] = "ABCD";
//	return str;
//}

//编译正常，返回的是常量区数据
const char *my_return_value4() {
	const char *str = "ABCD";
	return str;
}

DEF_test(alignment) {

	DEF_case(mem_alignment_by_4bytes) {
		EXPECT_EQ(sizeof(Un4), 5 * 4);
		EXPECT_EQ(sizeof(Xx4), 4 + 20 + 4 + 4);

		EXPECT_EQ(sizeof(Yy4), 4+5*4+8+4);
	}

	DEF_case(mem_alignment_by_2bytes) {
		EXPECT_EQ(sizeof(Un2), 5 * (2 + 2));
		EXPECT_EQ(sizeof(Xx2), 2 + 20 + (2 + 2) + 2);
	}

	DEF_case(mem_alignment_by_1bytes) {
		EXPECT_EQ(sizeof(Un1), 5 * 4);
		EXPECT_EQ(sizeof(Xx1), 1 + 20 + 4 + 1);
	}

	DEF_case(compare_addr) {
		unsigned char *p1;
		unsigned long *p2;
		p1 = (unsigned char *)0x801000;
		p2 = (unsigned long *)0x810000;

		EXPECT_EQ(p1 + 1, (unsigned char *)0x801001);
		EXPECT_EQ(p1 + 2, (unsigned char *)0x801002);
		EXPECT_EQ(p1 + 3, (unsigned char *)0x801003);
		EXPECT_EQ(p1 + 4, (unsigned char *)0x801004);
		EXPECT_EQ(p1 + 5, (unsigned char *)0x801005);

		
		if (4 == sizeof(p2)) {
			//以下是32机器上
			EXPECT_EQ(p2 + 1, (unsigned long *)0x810004);
			EXPECT_EQ(p2 + 2, (unsigned long *)0x810008);
			EXPECT_EQ(p2 + 3, (unsigned long *)0x81000C);
			EXPECT_EQ(p2 + 4, (unsigned long *)0x81000C);
			EXPECT_EQ(p2 + 5, (unsigned long *)0x810014);
		} else {
			//以下是64位机器上
			EXPECT_EQ(p2 + 1, (unsigned long *)0x810008);
			EXPECT_EQ(p2 + 2, (unsigned long *)0x810010);
			EXPECT_EQ(p2 + 3, (unsigned long *)0x810018);
			EXPECT_EQ(p2 + 4, (unsigned long *)0x810020);
			EXPECT_EQ(p2 + 5, (unsigned long *)0x810028);
		}
	}

	DEF_case(return_value) {
		/*EXPECT_NE(NULL, my_return_value1());
		COUT << "my_return_value1: [" << my_return_value1() << "]";

		EXPECT_NE(NULL, my_return_value2());
		COUT << "my_return_value2: [" << my_return_value2() << "]";

		EXPECT_NE(NULL, my_return_value3());
		COUT << "my_return_value3: [" << my_return_value3() << "]";*/

		EXPECT(NULL != my_return_value4());
		COUT << "my_return_value4: [" << my_return_value4() << "]";
	}
}

}
