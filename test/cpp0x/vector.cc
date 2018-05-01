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

//#1 ��ǰ�����㹻�Ŀռ��Ա��ⲻ��Ҫ�����·���͸�������
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

//#2 ʹ�� shrink_to_fit() �ͷ� vector ռ�õ��ڴ棬 �C clear() �� erase() �����ͷ��ڴ档
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

//#3 �������߿����� vector ��ʱ��Ӧ��ʹ�ø�ֵ������ insert() ��push_back().
// ��ֵ�ǳ���Ч�ʣ���Ϊ��֪��Ҫ������ vector �ж��Ȼ��ֻ��Ҫͨ���ڴ����һ���Կ��� vector �ڲ��Ļ���
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

//#4 ���� std::vector Ԫ�ص�ʱ�򣬱���ʹ�� std::vector::at() ������

//#5 ���������� vector ǰ������Ԫ��

//#6 ���� vector ����Ԫ�ص�ʱ��ʹ�� emplace_back() ������ push_back()��

}//namespace test