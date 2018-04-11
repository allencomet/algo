#ifndef __DESIGN_OBSERVER_H
#define __DESIGN_OBSERVER_H

#include "../util/util.h"
#include <string>
#include <map>

namespace design {

//观察者模式：定义对象之间的一种一对多的关系，当目标对象发生变化时，观察者都将得到通知产生相应的变化

class Observer;

//目标对象
class Subject {
public:
	Subject() {}
	virtual ~Subject() {}

	virtual void notify() = 0;
	virtual void attach(Observer *) = 0;
	virtual void detach(Observer *) = 0;
};

//一群观察者对象
class Observer {
public:
	Observer() {}
	virtual ~Observer() {}

	virtual void update() = 0;
	virtual std::string id() = 0;
};

class Topic :public Subject {
public:
	Topic() {}
	virtual ~Topic() {}

	virtual void notify() {
		for (std::map<std::string, Observer *>::iterator it = _observers.begin();
			it != _observers.end(); ++it) {
			(it->second)->update();
		}
		COUT << "there are " << _observers.size() << " have been update...";
	}
	virtual void attach(Observer *ob) {
		if (NULL == ob) return;
		_observers.insert(std::make_pair(ob->id(),ob));
	}
	virtual void detach(Observer *ob) {
		if (NULL == ob) return;
		_observers.erase(ob->id());
	}
private:
	std::map<std::string,Observer *> _observers;
};

class Subscriber:public Observer {
public:
	Subscriber(const std::string &id) :_id(id) {}
	~Subscriber() {}

	virtual void update() {
		COUT << _id << " update...";
	}

	virtual std::string id() {
		return _id;
	}
private:
	std::string _id;
};

}

#endif