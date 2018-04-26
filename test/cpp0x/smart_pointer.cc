#include "../../util//util.h"

#include <vector>
#include <memory>

namespace test {

class B;
class A {
public:
	A() {
		COUT << "A default constructor";
	}

	~A() {
		COUT << "A destructor";
	}
	std::shared_ptr<B> _bptr;
};

class B {
public:
	B() {
		COUT << "B default constructor";
	}
	~B() {
		COUT << "B destructor";
	}
	std::shared_ptr<A> _aptr;
};

class CorrectB;
class CorrectA {
public:
	CorrectA() {
		COUT << "CorrectA default constructor";
	}
	~CorrectA() {
		COUT << "CorrectA destructor";
	}
	std::weak_ptr<CorrectB> _bptr;
};

class CorrectB {
public:
	CorrectB() {
		COUT << "CorrectB default constructor";
	}
	~CorrectB() {
		COUT << "CorrectB destructor";
	}
	std::weak_ptr<CorrectA> _aptr;
};
	

DEF_test(bad_solution) {

	DEF_case(its_ok) {
		COUT << "its_ok";
		A a;
		B b;
	}
	
	//ѭ������
	DEF_case(its_not_ok) {
		COUT << "its_not_ok";
		std::shared_ptr<A> aptr = std::make_shared<A>();
		std::shared_ptr<B> bptr = std::make_shared<B>();
		aptr->_bptr = bptr;
		bptr->_aptr = aptr;
	}
}

DEF_test(correct_solution) {

	DEF_case(its_ok) {
		CorrectA a;
		CorrectB b;
	}

	// ����ѭ������
	DEF_case(its_also_ok) {
		std::shared_ptr<CorrectA> aptr = std::make_shared<CorrectA>();
		std::shared_ptr<CorrectB> bptr = std::make_shared<CorrectB>();
		aptr->_bptr = bptr;
		bptr->_aptr = aptr;
	}
}

DEF_test(use_weak_ptr) {
	DEF_case(expired) {
		std::shared_ptr<CorrectA> aptr = std::make_shared<CorrectA>();
		std::weak_ptr<CorrectA> wp(aptr);
		EXPECT_EQ(1, aptr.use_count());
		aptr.reset();
		EXPECT_EQ(0, aptr.use_count());
		EXPECT_EQ(true, wp.expired());
	}
	

	DEF_case(lock) {
		std::shared_ptr<CorrectA> aptr = std::make_shared<CorrectA>();
		std::weak_ptr<CorrectA> wp(aptr);

		std::shared_ptr<CorrectA> temp = wp.lock();
		EXPECT(nullptr != temp);
		
		aptr.reset();
		EXPECT(nullptr != wp.lock());	// ��Ϊtemp������

		temp = wp.lock();	//��ʵ�����Լ���ֵ���Լ�
		
		EXPECT(1 == temp.use_count());	//��Ϊ����ֻ��temp�Լ���ʹ��

		temp.reset();

		EXPECT(nullptr == wp.lock());	// ��ΪΨһʹ����temp�Ѿ�����ӵ����
	}
}



}//namespace test