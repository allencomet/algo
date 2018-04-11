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


//使用模板方式的策略模式
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

//使用指针设置实现的策略模式
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