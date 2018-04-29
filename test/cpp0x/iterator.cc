#include "../../util/util.h"

#include <iostream>     // std::cout
#include <iterator>     // std::make_move_iterator
#include <vector>       // std::vector
#include <string>       // std::string
#include <algorithm>    // std::copy

namespace test {

// ��ôc++11��std::make_move_iterator�������ó��������Խ�һ����ͨ����������oldv.begin()��ת��Ϊ"moveʽ������"��
// ���std::copy������������ȫ�����ݵ����ã�move��������ͬʱȡ�������������ݵĳ���Ȩ�������c++11���ĸ�����
// �ݿ�����ʽ��
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
		dst.begin());	// ע��������ݵ��������������ܹ�������ô��Ԫ�أ������ý���
	src.clear();
}

DEF_test(copy_performance) {	// -- 795ms
	std::copy(src.begin(),src.end(),dst.begin());
}

DEF_test(swap_performance) {	// -- 0ms	�ٶ����
	std::swap(src, dst);
}

}//namespace test