#include "../../util/util.h"

#define BOOST_SYSTEM_NO_DEPRACTED
#include <boost/pool/pool.hpp>
#include <boost/pool/object_pool.hpp>

namespace test {

//-------------------------------------------------
//--------------        pool		---------------
//-------------------------------------------------

// ֻ��������ͨ������������int��double�ȵ��ڴ�أ�
// �������ڸ��ӵ���Ͷ�����Ϊֻ�����ڴ棬������
// ���캯�������ʱ��������Ҫobject_pool

// pool������ʹ�ã�������c�е�mallocһ�������ڴ棬Ȼ������ʹ�á�
// �����������Ҫ�󣬷��򲻱ضԷ�����ڴ����free�ͷţ�pool��ܺ�
// �Ĺ����ڴ�

// pool�ڷ����ڴ��ǲ����׳��쳣��������Ҫ�Լ��ֶ��жϻ�ȡ��ָ���Ƿ�Ϊnull
DEF_test(pool) {
	boost::pool<> pl(sizeof(int));

	int *p = static_cast<int*>(pl.malloc());// �����ڴ�
	EXPECT(pl.is_from(p));// ���Թ���

	pl.free(p);

	for (int i = 0; i < 100; ++i) {
		pl.ordered_malloc(10);// �����ͬʱ�ϲ�����������������������10���ڴ棩
	}
}

//-------------------------------------------------
//--------------    object_pool		---------------
//-------------------------------------------------

DEF_test(object_pool) {
	boost::object_pool<std::string> pl;

	// auto ��Ϊstd::string *
	auto p = pl.malloc(); // ����һ��ԭʼ�ڴ��
	EXPECT(pl.is_from(p));// ���Թ���

	p = pl.construct("allen");	// �������
	std::cout << *p << std::endl;
}//���д����Ķ��������ﶼ�ᱻ��ȷ���������ͷ��ڴ�

}//namespace test