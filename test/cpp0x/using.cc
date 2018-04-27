#include "../../util/util.h"

#include <iostream>
#include <iomanip>	// for std::setprecision & std::setw
#include <random>
#include <cmath>

namespace test {

// ��������ģ��
template<typename ReturnType, typename Type>
using CompareOperator = std::function<ReturnType(Type &, Type &)>;

DEF_test(using_typedef) {
	typedef int(*fun)(int, int);	// c99����ָ�����
	using fun_alias = int(*)(int, int);	// c11ʹ��using��������

	typedef std::shared_ptr<safe::Thread> ThreadPtr;
	using ThreadPtr_alias = std::shared_ptr<safe::Thread>;

	using reply_msg = std::tuple<int32, std::string>;
}

DEF_test(random) {
	// random_device�ṩ()������, ��������һ��min()��max()֮���һ������.
	std::random_device rd;
	for (int n = 0; n < 20000; ++n)
		COUT << rd();
}

DEF_test(engine) {
	std::random_device rd;

	//��׼���������������������ͷֲ�������.�����������������,�ֲ������ض��ֲ��������(����ƽ���ֲ�,��̫�ֲ���).
	std::mt19937 mt(rd());
	for (int n = 0; n < 10; n++)
		COUT << mt();
}

// ƽ���ֲ�
DEF_test(dis) {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(1, 6);
	for (int n = 0; n<10; ++n)
		std::cout << dis(gen) << ' ';
	std::cout << '\n';
}

// ��̫�ֲ�
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
		funEx fe = std::bind(query, std::cref(qry), std::cref(aux)); //��������ַ�ʽ��Ҫת��funEx
		RspMsg rsp = f();
		COUT << "rsp number: " << std::get<0>(rsp) << ",rsp msg: " << std::get<1>(rsp);

		rsp = fe();
		COUT << "rsp number: " << std::get<0>(rsp) << ",rsp msg: " << std::get<1>(rsp);
	}
}

}//namespace test