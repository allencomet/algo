#include "../../util/util.h"

// pool组件本身没有外部依赖，但头文件pool_fwd.hpp里
// 的singleton_pool间接依赖了boost.system库，为了避免
// 不必要的链接错误，在include语句前定义如下宏
#define BOOST_SYSTEM_NO_DEPRECATED
#include <boost/pool/pool.hpp>
#include <boost/pool/object_pool.hpp>

/*
	安装boost库：./b2 --buildtype=complete install
*/

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

 // 默认情况下，在使用object_pool的construct的时候我们只能最多使用3个参数
 // 来创建对象，这样就对我们的适用范围造成了一定的限制，我们可以顶一个c++11
 // 辅助模板函数，支持任意数量的参数，彻底解决这个问题
template<typename P,typename ...Args>
inline typename P::element_type* 
construct(P& p, Args&& ...args) {
	typename P::element_type* mem = p.malloc();
	assert(mem != 0);
	new (mem) typename P::element_type(std::forward<Args>(args)...);//c++11完美转发
	return mem;
}

class DemonClass {
public:
	DemonClass(int a, int b, int c, int d):_a(a),_b(b),_c(c),_d(d) {
		COUT << "default constructor...";
	}
	~DemonClass() {
		COUT << "destructor...";
	}

	inline int a() const {
		return _a;
	}

	inline int b() const {
		return _b;
	}

	inline int c() const {
		return _c;
	}

	inline int d() const {
		return _d;
	}
private:
	int _a;
	int _b;
	int _c;
	int _d;
};

DEF_test(object_pool_ex) {
	boost::object_pool<DemonClass> pl;
	auto d = construct(pl, 1, 2, 3, 4);
	EXPECT_EQ(1, d->a());
	EXPECT_EQ(2, d->b());
	EXPECT_EQ(3, d->c());
	EXPECT_EQ(4, d->d());
}

}//namespace test