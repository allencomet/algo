#include "../../util/util.h"

#include<thread>
#include<chrono>

//线程函数
void func(int a, int b, int c) {
	std::this_thread::sleep_for(std::chrono::seconds(3));
	COUT << a << " " << b << " " << c;
}

namespace test {

DEF_test(test_thread_01) {
	//创建线程对象t1,绑定线程函数为func
	std::thread t1(func, 1, 2, 3);
	//输出t1的线程ID
	std::cout << "ID:" << t1.get_id() << std::endl;
	//等待t1线程函数执行结束
	t1.join();
}

DEF_test(test_thread_02) {
	std::thread t2(func, 2, 3, 4);
	//后台执行t2的线程函数,并且不会因为main函数结束时,线程函数未执行完而产生异常
	t2.detach();
	std::cout << "after t2 ,main is runing" << std::endl;
}

}//namespace test