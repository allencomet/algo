#include "../../util/util.h"

#include <algorithm>
#include <string>

//ADL: Arguement Dependent name Lookup
namespace test {

//Rvalue references are a feature of C++ that was added with the C++11 standard. 
//The syntax of an rvalue reference is to add && after a type.
template<typename Value>
class DynArray {
public:
	// (default) constructor
	DynArray(size_t size = 0) 
		:_size(size),
		_data(_size == 0 ? nullptr : new Value[_size]) {
		COUT << "default-constructor";
	}
	~DynArray() {
		COUT << "destructor";
		if (nullptr != _data) {
			delete[] _data;
			_data = nullptr;
		}
	}

	// copy-constructor
	// it works by using the copy-constructor's functionality to create a local copy of the data
	// then takes the copied data with a swap function, swapping the old data with the new data. 
	// The temporary copy then destructs, taking the old data with it. We are left with a copy of the new data.
	DynArray(const DynArray &arr)
		:_size(arr._size), 
		_data(_size == 0 ? nullptr : new Value[_size]) {
		COUT << "copy-constructor";
		// note that this is non-throwing, because of the data  
		// types being used; more attention to detail with regards  
		// to exceptions must be given in a more general case, however
		std::copy(arr._data, arr._data + _size, _data);
	}

	// move-constructor
	DynArray(DynArray &&arr)
		:DynArray(){ // initialize via default constructor, C++11 only
		COUT << "move-constructor";
		swap(*this, arr);
	}

	// copy-and-swap-idiom
	friend void swap(DynArray &left, DynArray &right) {
		// enable ADL (not necessary in our case, but good practice)  
		using std::swap;
		COUT << "self swap";
		// by swapping the members of two classes,  
		// the two classes are effectively swapped 
		swap(left._size, right._size);
		swap(left._data, right._data);
	}

	//// arr initialize via copy-constructor
	//DynArray &operator=(DynArray arr) {	//copy-and-swap 无法区分右值引用
	//	COUT << "copy assignment operator";
	//	swap(*this, arr);
	//	return *this;
	//}

	DynArray &operator=(const DynArray &arr) {
		COUT << "copy assignment operator";
		_size = arr._size;
		std::copy(arr._data, arr._data + _size, _data);
		return *this;
	}

	DynArray &operator=(DynArray &&arr) {
		COUT << "move assignment operator";
		//if (this != &arr) {
		//	this->_size = arr._size;
		//	this->_data = arr._data;
		//	arr._size = 0;
		//	arr._data = nullptr;
		//}
		swap(*this, arr);
		return *this;
	}

	size_t size() const {
		return _size;
	}

	Value* data() const {
		return _data;
	}

private:
	size_t _size;
	Value *_data;
};

DEF_test(rref) {	
	DynArray<std::string> arr(1 << 10);
	//DynArray<std::string> rarr = std::move(DynArray<std::string>());
	//DynArray<std::string> larr = DynArray<std::string>();

	for (int i = 0; i < 2; ++i) {
		//DynArray<std::string> temp(DynArray<std::string>(10));	// How does it work? --default constructor
		//DynArray<std::string> temp(arr);	// How does it work? --copy constructor
		//DynArray<std::string> temp(std::move(DynArray<std::string>()));	// How does it work? --move constructor
		//arr = std::move(DynArray<std::string>());	// How does it work? --assignment operator
		arr = DynArray<std::string>();	// How does it work? --assignment operator
	}

	/*
	DynArray<char> str(1 << 10);
	for (int i = 0; i < 2; ++i) {
		DynArray<char> temp(DynArray<char>(10));	// How does it work? --default constructor
	}

	std::string old_str("abc");
	//std::string new_str = std::move("abc");
	std::string new_str = std::move(old_str);
	COUT << "after moveing: " << new_str << ",old_str: " << old_str;
	*/
}

std::vector<std::string> v(1 << 25);

DEF_test(copy) {
	try {
		std::vector<std::string> temp = v;
	} catch (std::exception &e) {
		CERR << e.what();
	}
}

DEF_test(move) {
	try {
		std::vector<std::string> temp = std::move(v);
	} catch (std::exception &e) {
		CERR << e.what();
	}
}


template<typename T>
void PrintT(T& t) {
	COUT << "lvaue";
}

template<typename T>
void PrintT(T && t) {
	COUT << "rvalue";
}

template<typename T>
void TestForward(T && v) {
	PrintT(v);
	PrintT(std::forward<T>(v));
	PrintT(std::move(v));
}

DEF_test(forward) {
	//TestForward(1);	// lvalue rvalue rvalue

	// 被一个左值初始化后变成了一个左值引用
	int x = 1;
	//TestForward(x);	// lvalue lvalue rvalue

	// 
	TestForward(std::forward<int>(x));	// lvalue rvalue rvalue
}

class MyString {
public:
	MyString() {
		_data = NULL;
		_len = 0;
	}

	virtual ~MyString() {
		if (_data) {
			delete[] _data;
			_data = nullptr;
		}
	}

	MyString(const char* p) {
		_len = strlen(p);
		copy_data(p);
	}

	MyString(const MyString& str):_len(str._len) {
		std::copy(str._data, str._data + _len, _data);
		std::cout << "Copy Constructor is called! source: " << str._data << std::endl;
	}

	// move-constructor
	MyString(MyString&& str) {
		std::cout << "Move Constructor is called! source: " << str._data << std::endl;
		//_len = str._len;
		//_data = str._data; //避免了不必要的拷贝
		//str._len = 0;
		//str._data = NULL;
		swap(*this, str);
	}

	MyString& operator=(const MyString& str) {
		_len = str._len;
		std::copy(str._data, str._data + _len, _data);
		std::cout << "Copy Assignment is called! source: " << str._data << std::endl;
		return *this;
	}

	//// copy-and-swap 编译器则无法区分右值引用
	//MyString& operator=(MyString str) {
	//	swap(*this, str);
	//	std::cout << "Copy Assignment is called! source: " << str._data << std::endl;
	//	return *this;
	//}

	MyString& operator=(MyString&& str) {
		std::cout << "Move Assignment is called! source: " << str._data << std::endl;
		if (this != &str) {
			if (nullptr != _data) delete[] _data;	//释放原空间
			_len = str._len;
			_data = str._data; //避免了不必要的拷贝
			str._len = 0;
			str._data = NULL;
		}
		//swap(*this, str);	//在这里不能用swap，必须保持原对象失效
		return *this;
	}

	//获取长度
	size_t size() const {
		return _len;
	}

	//获取C字符串
	const char* c_str() const {
		return _len != 0 ? _data : '\0';
	}

	//输出
	friend std::ostream& operator<<(std::ostream &os, MyString &str) {
		os << str._data;
		return os;
	}

	// copy-and-swap-idiom
	friend void swap(MyString &left, MyString &right) {
		// enable ADL (not necessary in our case, but good practice)  
		using std::swap;
		COUT << "self swap";
		// by swapping the members of two classes,  
		// the two classes are effectively swapped 
		std::swap(left._len, right._len);
		std::swap(left._data, right._data);
	}
private:
	char* _data;
	size_t   _len;

	void copy_data(const char *s) {
		_data = new char[_len + 1];
		memcpy(_data, s, _len);
		_data[_len] = '\0';
	}
};

DEF_test(mystr) {
	//MyString &&str1 = std::move("abcd");
	MyString strOrigin("ab");
	MyString strDest("cd");

	std::cout << "Origin: " << strOrigin.c_str() << ",Dest: " << strDest.c_str() << std::endl;

	strDest = std::move(strOrigin);
	strDest = "abcd";

	std::cout << "Dest: " << strDest << ",Origin: " << strOrigin << "\n" << std::endl;
	std::cout << std::endl;
}

}//namespace test