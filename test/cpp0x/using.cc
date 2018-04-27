#include "../../util/util.h"

#include <iostream>
#include <iomanip>	// for std::setprecision & std::setw
#include <random>
#include <cmath>

#include <unordered_map>
#include <unordered_set>

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
using AuxMsg = std::tuple<int32, std::string>;
RspMsg query(const QryMsg &msg,const AuxMsg &aux) {
	COUT << "msg id: " << std::get<0>(msg) << ",msg data: " << std::get<1>(msg);
	COUT << "aux id: " << std::get<0>(aux) << ",aux data: " << std::get<1>(aux);
	return std::make_tuple(0, "successful");
}

DEF_test(std_ref) {
	using fun = std::function<RspMsg(const QryMsg &, const AuxMsg &)>;
	using funEx = std::function<RspMsg()>;

	DEF_case(case01) {
		QryMsg qry = std::make_tuple(29, "hello");
		AuxMsg aux = std::make_tuple(-1, "auxilary");
		fun f = std::bind(query, std::placeholders::_1, std::placeholders::_2);
		//const RspMsg rsp = f(std::cref(qry));
		const RspMsg rsp = f(qry, aux);
		COUT << "rsp number: " << std::get<0>(rsp) << ",rsp msg: " << std::get<1>(rsp);
	}
	
	DEF_case(case02) {
		QryMsg qry = std::make_tuple(29, "hello");
		AuxMsg aux = std::make_tuple(-1, "auxilary");
		auto f = std::bind(query, std::cref(qry), std::cref(aux));
		funEx fe = std::bind(query, std::cref(qry), std::cref(aux)); //如果用这种方式需要转成funEx
		RspMsg rsp = f();
		COUT << "rsp number: " << std::get<0>(rsp) << ",rsp msg: " << std::get<1>(rsp);

		rsp = fe();
		COUT << "rsp number: " << std::get<0>(rsp) << ",rsp msg: " << std::get<1>(rsp);
	}
}




class Monitor {
public:
	Monitor() {}
	~Monitor() {}

	void init_function(int32 func) {
		auto it = _fun_info.find(func);
		if (it == _fun_info.end()) {
			FunInfo info;
			info._last_time = sys::utc.sec();
			++info._use_count;
			_fun_info.insert(std::make_pair(func, info));
		} else {
			FunInfo &info = it->second;
			info._last_time = sys::utc.sec();
			++info._use_count;
		}
	}

	void set_end_time(int32 func, int64 val) {
		auto it = _fun_info.find(func);
		if (it != _fun_info.end()) {
			FunInfo &info = it->second;

			info._max_time = val > info._max_time ? val : info._max_time;
			info._min_time = val < info._max_time ? val : info._min_time;
			info._use_time = (info._use_time*(info._use_count - 1) + val) / (info._use_count);//int64保存不下时可能会溢出
		}
	}

	void show_info(int32 func) {
		auto it = _fun_info.find(func);
		if (it != _fun_info.end()) {
			FunInfo &info = it->second;
			COUT << "use count: " << info._use_count << "times, use time: " << info._use_time << "ms, max time: " <<
				info._max_time << "ms, min time: " << info._min_time << "ms, last time: " << info._last_time;
		}
	}
private:
	class FunInfo {
	public:
		FunInfo() :_use_count(0), _use_time(0), _max_time(0), _min_time(0), _last_time(0){}
		~FunInfo() = default;

		int32 _use_count;	// 调用次数
		int32 _use_time;	// 调用平均耗时 ms
		int32 _max_time;	// 调用最长耗时 ms
		int32 _min_time;	// 调用最短耗时 ms
		int64 _last_time;	// 调用最后时间戳 s
	};

	std::unordered_map<int32, FunInfo> _fun_info;	// count of use

	DISALLOW_COPY_AND_ASSIGN(Monitor);
};

class MonGuard {
public:
	MonGuard(Monitor &mon,int32 func):_monitor(mon),_func(func){
		_monitor.init_function(_func);
		_t.restart();
	}

	~MonGuard() {
		_monitor.set_end_time(_func, _t.ms());
	}
private:
	Monitor &_monitor;
	int32 _func;
	sys::timer _t;
};

class BaseRequest {
public:
	BaseRequest() :flow(0), func(0), type(0) {}
	~BaseRequest() = default;

	int32 flow;	// 流水号
	int32 func;
	int32 type; // 消息类型
	std::string content;
};

static Monitor kMonitor;

std::vector<std::string> vv(1 << 25);

void invoke_function(Monitor &mon, const BaseRequest &req) {
	MonGuard g(mon, req.func);
	// do something
	std::vector<std::string> temp = vv;
	COUT << "flow: " << req.flow << ", copy done";
}

DEF_test(bind_usage) {
	auto bind_fun = std::bind(invoke_function, std::ref(kMonitor), std::placeholders::_1);

	BaseRequest req;
	req.func = 100;
	for (int i = 0; i < 100; ++i) {
		req.flow = i + 1;
		bind_fun(req);
	}
	kMonitor.show_info(req.func);
}

}//namespace test