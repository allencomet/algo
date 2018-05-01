#include "../../util/util.h"

#define BOOST_SYSTEM_NO_DEPRACTED
#include <boost/pool/pool.hpp>
#include <boost/pool/object_pool.hpp>

namespace test {

//-------------------------------------------------
//--------------        pool		---------------
//-------------------------------------------------

// 只能用于普通数据了性例如int、double等的内存池，
// 不能用于复杂的类和对象，因为只分配内存，不调用
// 构造函数，这个时候我们需要object_pool

// pool很容易使用，可以像c中的malloc一样分配内存，然后随意使用。
// 除非有特殊的要求，否则不必对分配的内存调用free释放，pool会很好
// 的管理内存

// pool在分配内存是不会抛出异常，所以需要自己手动判断获取的指针是否为null
DEF_test(pool) {
	boost::pool<> pl(sizeof(int));

	int *p = static_cast<int*>(pl.malloc());// 分配内存
	EXPECT(pl.is_from(p));// 测试归属

	pl.free(p);

	for (int i = 0; i < 100; ++i) {
		pl.ordered_malloc(10);// 分配的同时合并空闲链表（在这里连续分配10块内存）
	}
}

//-------------------------------------------------
//--------------    object_pool		---------------
//-------------------------------------------------

DEF_test(object_pool) {
	boost::object_pool<std::string> pl;

	// auto 即为std::string *
	auto p = pl.malloc(); // 分配一个原始内存块
	EXPECT(pl.is_from(p));// 测试归属

	p = pl.construct("allen");	// 构造对象
	std::cout << *p << std::endl;
}//所有创建的对象在这里都会被正确的析构、释放内存

}//namespace test