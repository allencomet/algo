#ifndef __DESIGN_SINGLETON_H
#define __DESIGN_SINGLETON_H

#include "../util/util.h"

namespace design {

//懒汉式
class LazySingleton {
public:
	static LazySingleton* instance() {
		if (NULL == _instance) _instance = new LazySingleton();
		return _instance;
	}
private:
	LazySingleton() {}
	~LazySingleton() {}
	static LazySingleton* _instance;

	DISALLOW_COPY_AND_ASSIGN(LazySingleton);
};
LazySingleton* LazySingleton::_instance = NULL;

//线程安全懒汉式(通过double-check来保证thread safety)
class SafeLazySingleton {
public:
	static SafeLazySingleton* instance() {
		if (NULL == _instance) {
			pthread_mutex_lock(&_lock);
			if (NULL == _instance) _instance = new SafeLazySingleton();
			pthread_mutex_unlock(&_lock);
		}
		return _instance;
	}
private:
	SafeLazySingleton() {}
	~SafeLazySingleton() {}

	class Garbo {
	public:
		~Garbo() {
			if (NULL != SafeLazySingleton::_instance) {
				delete SafeLazySingleton::_instance;
				SafeLazySingleton::_instance = NULL;
			}
		}
	};
	static Garbo _garbo;

	static SafeLazySingleton* _instance;
	static pthread_mutex_t _lock;

	DISALLOW_COPY_AND_ASSIGN(SafeLazySingleton);
};

SafeLazySingleton* SafeLazySingleton::_instance = NULL;
pthread_mutex_t SafeLazySingleton::_lock = PTHREAD_MUTEX_INITIALIZER;



//饿汉式
class StarvingSingleton {
public:
	static StarvingSingleton* instance() {
		static StarvingSingleton _instance;
		return &_instance;
	}

private:
	StarvingSingleton() {}
	~StarvingSingleton() {}

	DISALLOW_COPY_AND_ASSIGN(StarvingSingleton);
};


}

#endif