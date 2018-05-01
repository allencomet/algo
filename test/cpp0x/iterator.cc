#include "../../util/util.h"

#include <iostream>     // std::cout
#include <iterator>     // std::make_move_iterator
#include <vector>       // std::vector
#include <string>       // std::string
#include <algorithm>    // std::copy

namespace test {

// 那么c++11的std::make_move_iterator将派上用场，它可以将一个普通迭代器，如oldv.begin()，转化为"move式迭代器"，
// 配合std::copy，将老容器内全部数据的引用，move给新容器同时取消老容器对数据的持有权。这就是c++11风格的高速数
// 据拷贝方式。
DEF_test(make_move_iterator) {
	std::vector<std::string> foo(3);
	std::vector<std::string> bar{ "one","two","three" };

	std::copy(std::make_move_iterator(bar.begin()),
		std::make_move_iterator(bar.end()),
		foo.begin());

	// bar now contains unspecified values; clear it:
	bar.clear();

	std::cout << "foo:";
	for (std::string& x : foo) std::cout << ' ' << x;
	std::cout << '\n';
}

static std::vector<std::string> src(1 << 25,"allen");
static std::vector<std::string> dst(1 << 25);
DEF_test(make_move_iterator_performance) {	// -- 341ms
	std::copy(std::make_move_iterator(src.begin()),
		std::make_move_iterator(src.end()),
		dst.begin());	// 注意接收数据的容器必须至少能够容纳这么多元素，否则用交换
	src.clear();
}

DEF_test(copy_performance) {	// -- 795ms
	std::copy(src.begin(),src.end(),dst.begin());
}

DEF_test(swap_performance) {	// -- 0ms	速度最快
	std::swap(src, dst);
}

}//namespace test