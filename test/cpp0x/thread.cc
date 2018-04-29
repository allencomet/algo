#include "../../util/util.h"

#include<thread>
#include<chrono>

//�̺߳���
void func(int a, int b, int c) {
	std::this_thread::sleep_for(std::chrono::seconds(3));
	COUT << a << " " << b << " " << c;
}

namespace test {

DEF_test(test_thread_01) {
	//�����̶߳���t1,���̺߳���Ϊfunc
	std::thread t1(func, 1, 2, 3);
	//���t1���߳�ID
	std::cout << "ID:" << t1.get_id() << std::endl;
	//�ȴ�t1�̺߳���ִ�н���
	t1.join();
}

DEF_test(test_thread_02) {
	std::thread t2(func, 2, 3, 4);
	//��ִ̨��t2���̺߳���,���Ҳ�����Ϊmain��������ʱ,�̺߳���δִ����������쳣
	t2.detach();
	std::cout << "after t2 ,main is runing" << std::endl;
}

}//namespace test