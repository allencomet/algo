#include "../util/util.h"

#include <string>

#define ALLEN_say(content) "ALLEN_"#content

#define ALLEN_max(x,y) \
	if(x > y) {\
		std::cout << "[" << #x << ":" << x << "," << #y << ":" << y << "]: " << x << std::endl;\
	}else{\
		std::cout << "[" << #x << ":" << x << "," << #y << ":" << y << "]: " << y << std::endl;\
	}

#define STR(x) #x

class Programmer {
public:
	virtual std::string name() = 0;
};

class Programmers {
public:
	static Programmers* instance() {
		static Programmers programmers;
		return &programmers;
	}

	void add_programmer(Programmer *coder) {
		_group.push_back(coder);
	}

	void run() {
		for (std::vector<Programmer *>::iterator it = _group.begin();
			it != _group.end(); ++it) {
			COUT << "run: " << (*it)->name();
		}
	}
private:
	Programmers() {}
	~Programmers() {}

	std::vector<Programmer *> _group;

	DISALLOW_COPY_AND_ASSIGN(Programmers);
};

class CoderSaver {
public:
	CoderSaver(Programmer *base) {
		Programmers::instance()->add_programmer(base);
	}
};


#define DEF_coder(_name_) \
	class Coder##_name_: public ::Programmer{\
	  public:\
		Coder##_name_(const std::string str):_name(str){}\
		~Coder##_name_(){}\
		\
		virtual std::string name(){\
			return _name;\
		}\
		\
	  private:\
		std::string _name;\
	};\
	static CoderSaver SaveCoder_##_name_(new Coder##_name_(#_name_));


void run_coder() {
	Programmers::instance()->run();
}

namespace test {

DEF_coder(comet);
DEF_coder(allen);

DEF_test(macro) {
	COUT << ALLEN_say("COMET");		//ALLEN_"COMET"
	std::string COMET;
	COUT << ALLEN_say(COMET);		//ALLEN_COMET
	int a = 2, b = 7;
	ALLEN_max(a, b);

	std::string xxx;
	COUT << STR(xxx);

	run_coder();
}

}