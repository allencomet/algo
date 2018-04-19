#include "../util/util.h"

namespace test {

typedef enum PROJECT {
	kUnknow,
	kChinese,
	kMath,
	kEnglish
}PROJECT;

typedef struct Info {
	Info() {}
	Info(std::string n,float s):name(n),score(s){}
	std::string name;
	float score;
}Info;

const Info kUnknowInfo("Unknow", 0.0);

class Student {
public:
	Student() {}
	Student(const std::string &name, int age) :_name(name), _age(age) {}
	~Student() {}

	void set(PROJECT pro,float score) {
		std::map<PROJECT, Info>::iterator it = _projects.find(pro);
		if (_projects.end() == it) {
			Info info;
			//setting the name of project
			switch (pro) {
			case kChinese:
				info.name = "Chinese";
				break;
			case kMath:
				info.name = "Math";
				break; 
			case kEnglish:
				info.name = "English";
				break;
			default:
				info.name = "Other";
				break;
			}
			info.score = score;
			_projects.insert(std::make_pair(pro, info));
		} else {
			Info &info = it->second;
			info.score = score;
		}
	}

	void print(PROJECT pro) {
		switch (pro) {
		case kChinese:
		case kMath:
		case kEnglish:
			print(info(pro));
			break;
		case kUnknow:
		default:
			print();//defalut print all
			break;
		}
	}

	float score(PROJECT pro) const{
		std::map<PROJECT, Info>::const_iterator it = _projects.find(pro);
		if (_projects.end() != it) {
			const Info &info = it->second;
			return info.score;
		}
		return 0.0;
	}

	float average() const {
		float sum = 0.0;
		for (std::map<PROJECT, Info>::const_iterator it = _projects.begin(); 
			it != _projects.end(); ++it) {
			const Info &info = it->second;
			sum += info.score;
		}
		return sum / (float)_projects.size();
	}

	float sum() const {
		float temp = 0.0;
		for (std::map<PROJECT, Info>::const_iterator it = _projects.begin();
			it != _projects.end(); ++it) {
			const Info &info = it->second;
			temp += info.score;
		}
		return temp;
	}
private:
	void print(const Info &info) {
		std::cout << "name: " << info.name << ",score: " << info.score << std::endl;
	}
	void print() {
		for (std::map<PROJECT, Info>::const_iterator it = _projects.begin();
			it != _projects.end(); ++it) {
			print(it->second);
		}
	}

	const Info& info(PROJECT pro) const{
		std::map<PROJECT, Info>::const_iterator it = _projects.find(pro);
		if (_projects.end() != it) {
			return it->second;
		}
		return kUnknowInfo;
	}

	std::string _name;
	int _age;
	std::map<PROJECT, Info> _projects;

	DISALLOW_COPY_AND_ASSIGN(Student);
};

class EduManager {
public:
	EduManager() {}
	~EduManager() {}

	void run() {
		start_msg();
		process();
		end_msg();
	}
private:
	inline void start_msg() {
		std::cout << "Welcom to EduManager system..." << std::endl;
	}

	inline void end_msg() {
		std::cout << "Thank you for using EduManager system!" << std::endl;
	}

	inline void invalid_msg() {
		std::cout << "invalid option!" << std::endl;
	}

	inline void menu_msg() {
		std::cout << "---------------------------------------------\n";
		std::cout << "\t1.view students information\n";
		std::cout << "\t2.add students information\n";
		std::cout << "\t3.update students information\n";
		std::cout << "\t4.delete students information\n";
		std::cout << "\t5.exit\n";
		std::cout << "---------------------------------------------\n";
		std::cout << std::endl;
	}

	enum {
		kView,
		kAdd,
		kUpdate,
		kDelete,
		kExit
	};

	void process() {
		bool flag = false;
		for (;;) {
			menu_msg();
			int opt = 0;
			std::cin >> opt;
			switch (opt) {
			case kView:
				view_info();
				break;
			case kAdd:
				add_info();
				break;
			case kUpdate:
				update_info();
				break;
			case kDelete:
				del_info();
				break;
			case kExit:
				flag = exit_confirm();
				break;
			default:
				invalid_msg();
				break;
			}

			if (flag) break;
		}
	}

	void view_info() {
		std::cout << "\tview students information" << std::endl;

	}

	void add_info() {
		
	}

	void update_info() {
	
	}

	void del_info() {
	
	}

	bool exit_confirm() {
		std::cout << "would you like to exit EduManager?(y/n)";
		char c;
		std::cin >> c;
		if (c != 'y') return false;
		else return true;
	}

	std::map<std::string, Student *> _stus;

	DISALLOW_COPY_AND_ASSIGN(EduManager);
};

}