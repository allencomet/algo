#include "../../util/util.h"

#include <iostream>
#include <iomanip>	// for std::setprecision & std::setw
#include <random>
#include <cmath>

namespace test {

// 可以用于模板
template<typename ReturnType, typename Type>
using CompareOperator = std::function<ReturnType(Type &, Type &)>;

DEF_test(using_typedef) {
	typedef int(*fun)(int, int);	// c99函数指针别名
	using fun_alias = int(*)(int, int);	// c11使用using声明别名

	typedef std::shared_ptr<safe::Thread> ThreadPtr;
	using ThreadPtr_alias = std::shared_ptr<safe::Thread>;

	using reply_msg = std::tuple<int32, std::string>;
}

DEF_test(random) {
	// random_device提供()操作符, 用来返回一个min()到max()之间的一个数字.
	std::random_device rd;
	for (int n = 0; n < 20000; ++n)
		COUT << rd();
}

DEF_test(engine) {
	std::random_device rd;

	//标准把随机数抽象成随机数引擎和分布两部分.引擎用来产生随机数,分布产生特定分布的随机数(比如平均分布,正太分布等).
	std::mt19937 mt(rd());
	for (int n = 0; n < 10; n++)
		COUT << mt();
}

// 平均分布
DEF_test(dis) {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(1, 6);
	for (int n = 0; n<10; ++n)
		std::cout << dis(gen) << ' ';
	std::cout << '\n';
}

// 正太分布
DEF_test(hist) {
	std::random_device rd;
	std::mt19937 gen(rd());

	// values near the mean are the most likely
	// standard deviation affects the dispersion of generated values from the mean
	std::normal_distribution<> d(5, 2);

	std::map<int, int> hist;
	for (int n = 0; n<10000; ++n) {
		++hist[std::round(d(gen))];
	}
	for (auto p : hist) {
		std::cout << std::fixed << std::setprecision(1) << std::setw(2)
			<< p.first << ' ' << std::string(p.second / 200, '*') << '\n';
	}
}

#include <functional>

using RspMsg = std::tuple<int32, std::string>;
using QryMsg = std::tuple<int32, std::string>;
RspMsg query(const QryMsg &msg) {
	COUT << "msg id: " << std::get<0>(msg) << ",msg data: " << std::get<1>(msg);
	return std::make_tuple(0, "successful");
}

DEF_test(std_ref) {
	using fun = std::function<RspMsg(const QryMsg &)>;

	QryMsg qry = std::make_tuple(29, "hello");
	fun f = std::bind(query, std::placeholders::_1);
	const RspMsg rsp = f(std::cref(qry));
	COUT << "rsp number: " << std::get<0>(rsp) << ",rsp msg: " << std::get<1>(rsp);
}

}//namespace test