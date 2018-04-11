#ifndef __DESIGN_FACTORY_H
#define __DESIGN_FACTORY_H

#include "../util/util.h"

namespace design {

class ProductA {
public:
	ProductA() {}
	virtual ~ProductA() {}

	virtual void perform() = 0;
	virtual std::string id() = 0;
};

class ProductB {
public:
	ProductB() {}
	virtual ~ProductB() {}

	virtual void perform() = 0;
	virtual std::string id() = 0;
};

class A1 :public ProductA {
public:
	A1(const std::string &id):_id(id) {}
	virtual ~A1() {}

	virtual void perform() {
		COUT << "perform A1: " << _id;
	}

	virtual std::string id() {
		return _id;
	}
private:
	std::string _id;
};

class A2 :public ProductA {
public:
	A2(const std::string &id) :_id(id) {}
	virtual ~A2() {}

	virtual void perform() {
		COUT << "perform A2: " << _id;
	}

	virtual std::string id() {
		return _id;
	}
private:
	std::string _id;
};

class B1 :public ProductB {
public:
	B1(const std::string &id) :_id(id) {}
	virtual ~B1() {}

	virtual void perform() {
		COUT << "perform B1: " << _id;
	}

	virtual std::string id() {
		return _id;
	}
private:
	std::string _id;
};

class B2 :public ProductB {
public:
	B2(const std::string &id) :_id(id) {}
	virtual ~B2() {}

	virtual void perform() {
		COUT << "perform B2: " << _id;
	}

	virtual std::string id() {
		return _id;
	}
private:
	std::string _id;
};

class Factory {
public:
	Factory() {}
	virtual ~Factory() {}
	virtual ProductA *createProductA(const std::string &id) = 0;
	virtual ProductB *createProductB(const std::string &id) = 0;
};

class Factory1 :public Factory {
public:
	virtual ProductA *createProductA(const std::string &id) {
		return new A1(id);
	}

	virtual ProductB *createProductB(const std::string &id) {
		return new B1(id);
	}
};

class Factory2 :public Factory {
public:
	virtual ProductA *createProductA(const std::string &id) {
		return new A2(id);
	}

	virtual ProductB *createProductB(const std::string &id) {
		return new B2(id);
	}
};


}

#endif