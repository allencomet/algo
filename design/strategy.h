#ifndef __DESIGN_STRATEGY_H
#define __DESIGN_STRATEGY_H

#include "../util/util.h"
#include <map>

namespace design {

class Action {
public:
	Action() {}
	~Action() {}

	virtual void action() = 0;
};

class Swing:public Action{
public:
	void action() {
		COUT << "action: swing";
	}
};

class Run :public Action {
public:
	void action() {
		COUT << "action: run";
	}
};

class Jump :public Action {
public:
	void action() {
		COUT << "action: jump";
	}
};


//ʹ��ģ�巽ʽ�Ĳ���ģʽ
template<typename Algo>
class Strategy {
public:
	Strategy(Algo &algo):_algo(algo) {}
	~Strategy() {}

	void action() {
		_algo.action();
	}

private:
	Algo &_algo;
};

//ʹ��ָ������ʵ�ֵĲ���ģʽ
template<typename Action>
class StrategyPointer {
public:
	StrategyPointer() {}
	~StrategyPointer() {}

	void setStrategy(Action *ac) {
		_action = ac;
	}

	void action() {
		if (_action) _action->action();
	}

private:
	Action *_action;
};

}

#endif