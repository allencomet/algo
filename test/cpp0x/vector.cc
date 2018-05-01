#include "../../util/util.h"

#include <vector>

namespace {

// Test struct to be inserted/removed from vector
struct BigTestStruct {
	int iValue = 1;
	float fValue;
	long lValue;
	double dValue;
	char cNameArr[10];
	int iValArr[100];
};

// Helper function to populate the test vectors
void FillVector(std::vector<BigTestStruct>& testVector) {
	for (int i = 0; i < 10000; i++) {
		BigTestStruct bt;
		testVector.push_back(bt);
	}
}

}

namespace test {

//#1 提前分配足够的空间以避免不必要的重新分配和复制周期
DEF_test(vector_test01) {
	std::vector<BigTestStruct> testVector1;
	std::vector<BigTestStruct> testVector2;
	sys::timer t;
	FillVector(testVector1);
	std::cout << "Time to Fill Vector Without Reservation:" << t.us() << std::endl;
	t.restart();
	testVector2.reserve(10000);
	FillVector(testVector2);
	std::cout << "Time to Fill Vector With Reservation:" << t.us() << std::endl;
}

//#2 使用 shrink_to_fit() 释放 vector 占用的内存， – clear() 或 erase() 不会释放内存。
DEF_test(vector_test02) {
	using namespace std;
	std::vector<BigTestStruct> testVector1;
	std::vector<BigTestStruct> testVector2;
	FillVector(testVector1);
	size_t capacity = testVector1.capacity();
	cout << "Capacity Before Erasing Elements:" << capacity << endl;

	testVector1.erase(testVector1.begin(), testVector1.begin() + 3);
	capacity = testVector1.capacity();
	cout << "Capacity After Erasing 3 elements Elements:" << capacity << endl;

	testVector1.clear();
	capacity = testVector1.capacity();
	cout << "Capacity After clearing all elements:" << capacity << endl;

	testVector1.shrink_to_fit();
	capacity = testVector1.capacity();
	cout << "Capacity After shrinking the Vector:" << capacity << endl;
}

//#3 在填充或者拷贝到 vector 的时候，应该使用赋值而不是 insert() 或push_back().
// 赋值非常有效率，因为它知道要拷贝的 vector 有多大，然后只需要通过内存管理一次性拷贝 vector 内部的缓存
DEF_test(vector_test03) {
	using namespace std;
	vector<BigTestStruct> sourceVector, destinationVector;
	FillVector(sourceVector);

	// Assign sourceVector to destination vector
	sys::timer t;
	destinationVector = sourceVector;
	cout << "Assigning Vector :" << t.us() << endl;

	//Using std::vector::insert()
	vector<BigTestStruct> sourceVector1, destinationVector1;
	FillVector(sourceVector1);
	t.restart();
	destinationVector1.insert(destinationVector1.end(),
		sourceVector1.begin(),
		sourceVector1.end());
	cout << "Using insert() :" << t.us() << endl;
}

//#4 遍历 std::vector 元素的时候，避免使用 std::vector::at() 函数。

//#5 尽量避免在 vector 前部插入元素

//#6 在向 vector 插入元素的时候使用 emplace_back() 而不是 push_back()。

}//namespace test