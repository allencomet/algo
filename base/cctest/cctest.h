#pragma once

#include <string>
#include <vector>
#include <memory>
#include <iostream>

#include "../data_types.h"
#include "../cclog/cclog.h"
#include "../string_util.h"
#include "../thread_util.h"

#if __cplusplus >= 201103L

#include <memory>
#include <functional>

#else

#include <boost/smart_ptr.hpp>
#include <boost/bind.hpp>

#endif

DEC_uint32(timeout);  // timeout in ms for test cases
DEC_bool(a);          // run all tests if true

namespace cctest {

void init_cctest(int argc, char** argv);

void run_tests();

const std::vector<std::string>& args();

struct Test {
    Test() {}
    virtual ~Test() {}

    virtual void run() = 0;

    virtual const std::string& name() = 0;

    virtual bool on() = 0;

    DISALLOW_COPY_AND_ASSIGN(Test);
};

class TestRunner {
  public:
    TestRunner() {}
    ~TestRunner() {}

    void run_test(Test* test);

  private:
    void thread_fun() {
        _test->run();
        _ev.signal();
    }

#if __cplusplus >= 201103L
	std::unique_ptr<Test> _test;
	std::unique_ptr<safe::Thread> _thread;
#else
	boost::scoped_ptr<Test> _test;
	boost::scoped_ptr<safe::Thread> _thread;
#endif

    safe::SyncEvent _ev;

    DISALLOW_COPY_AND_ASSIGN(TestRunner);
};

struct Tests {
    static Tests* instance() {
        static Tests tests;
        return &tests;
    }

    void add_test(Test* test) {
        _tests.push_back(test);
    }

    void run_tests();

    void set_args(const std::vector<std::string>& args) {
        _args = args;
    }

    const std::vector<std::string>& get_args() const {
        return _args;
    }

  private:
    TestRunner _runner;     //将所有测试用例运行起来
    std::vector<Test*> _tests;//各测试用例
    std::vector<std::string> _args;//命令行参数列表

    Tests() {}
    ~Tests() {}
    DISALLOW_COPY_AND_ASSIGN(Tests);
};

struct TestSaver {
    TestSaver(Test* test) {
        cctest::Tests::instance()->add_test(test);
    }
};

} // namespace cctest

#define DEF_test(_name_) \
    DEF_bool(_name_, false, "enable " #_name_ " test if true"); \
    \
    struct Test_##_name_ : public ::cctest::Test { \
        Test_##_name_() : _name(#_name_) {} \
        virtual ~Test_##_name_() {} \
        \
        virtual void run(); \
        \
        virtual bool on() { \
            return FLG_##_name_; \
        } \
        \
        virtual const std::string& name() { \
            return _name; \
        } \
        \
        std::string current_case() { \
            int x = 12 - _current_case.size() % 12; \
            if (x < 2) x = 4; \
            return _current_case + std::string(x, ' '); \
        } \
        \
      private: \
        std::string _name; \
        std::string _current_case; \
    }; \
    \
    static ::cctest::TestSaver SaveTest_##_name_(new Test_##_name_()); \
    \
    void Test_##_name_::run()

//上面的新建一个测试用例类，并将这个测试用例类加入到单例的测试器里，这样每调用一次
//DEF_test宏，就会新建一个测试用例并添加到全局测试器里

//DEF_case必须放在DEF_test里面才能正常编译
#define DEF_case(_case_) _current_case = #_case_;

inline void set_green() {
    std::cout << "\033[0;32m";
    std::cout.flush();
}

inline void set_lgreen() {
    std::cout << "\e[1;32m";
    std::cout.flush();
}

inline void set_red() {
    std::cout << "\033[0;31m";
    std::cout.flush();
}

inline void set_lred() {
    std::cout << "\e[1;31m";
    std::cout.flush();
}

inline void set_black() {
    std::cout << "\e[0;30m";
    std::cout.flush();
}

inline void set_lblack() {
    std::cout << "\e[1;30m";
    std::cout.flush();
}

inline void set_lightblue() {
    std::cout << "\033[1;34m";
    std::cout.flush();
}

inline void set_brown() {
    std::cout << "\e[0;33m";
    std::cout.flush();
}

inline void set_yellow() {
    std::cout << "\e[1;33m";
    std::cout.flush();
}

inline void set_purple() {
    std::cout << "\e[0;35m";
    std::cout.flush();
}

inline void set_lpurple() {
    std::cout << "\e[1;35m";
    std::cout.flush();
}

inline void set_cyan() {
    std::cout << "\e[0;36m";
    std::cout.flush();
}

inline void set_lcyan() {
    std::cout << "\e[1;36m";
    std::cout.flush();
}

inline void set_gray() {
    std::cout << "\e[0;37m";
    std::cout.flush();
}

inline void set_white() {
    std::cout << "\e[1;37m";
    std::cout.flush();
}

inline void set_bold() {
    std::cout << "\e[1m";
    std::cout.flush();
}

inline void set_underline() {
    std::cout << "\e[4m";
    std::cout.flush();
}

inline void set_blink() {
    std::cout << "\e[5m";
    std::cout.flush();
}

inline void set_reverse() {
    std::cout << "\e[7m";
    std::cout.flush();
}

inline void set_hide() {
    std::cout << "\e[8m";
    std::cout.flush();
}

inline void set_clear() {
    std::cout << "\e[2J";
    std::cout.flush();
}

inline void set_clrline() {
    std::cout << "\r\e[K";
    std::cout.flush();
}

inline void reset_color() {
    std::cout << "\033[0m";
    std::cout.flush();
}



#define EXPECT(_cond_) \
    if (_cond_) {\
        set_green(); \
        std::cout << "test " << name() << ": " << current_case() \
            << "EXPECT(" << #_cond_ << ") ok" << std::endl; \
        reset_color(); \
    } else { \
        set_red(); \
        std::cout << "test " << name() << ": " << current_case() \
            << "EXPECT(" << #_cond_ << ") failed" << std::endl; \
        reset_color(); \
    }


#if __cplusplus >= 201103L

#define EXPECT_OP(_x_, _y_, _op_, _op_name_) \
{\
    auto __x = _x_; auto __y = _y_; \
    if (__x _op_ __y) { \
        set_green(); \
        std::cout << "test " << name() << ": " << current_case() \
            << "EXPECT_" << _op_name_ << "(" << #_x_ << ", " << #_y_ \
            << ") ok" << std::endl; \
        reset_color(); \
        \
    } else { \
        set_red(); \
        std::cout << "test " << name() << ": " << current_case() \
            << "EXPECT_" << _op_name_ << "(" << #_x_ << ", " << #_y_ \
            << ") failed: " << __x << " vs " << __y << std::endl; \
        reset_color(); \
    } \
}


#else

#define EXPECT_OP(_x_, _y_, _op_, _op_name_) \
{\
    if (_x_ _op_ _y_) { \
        set_green(); \
        std::cout << "test " << name() << ": " << current_case() \
            << "EXPECT_" << _op_name_ << "(" << #_x_ << ", " << #_y_ \
            << ") ok" << std::endl; \
        reset_color(); \
        \
    } else { \
        set_red(); \
        std::cout << "test " << name() << ": " << current_case() \
            << "EXPECT_" << _op_name_ << "(" << #_x_ << ", " << #_y_ \
            << ") failed: " << _x_ << " vs " << _y_ << std::endl; \
        reset_color(); \
    } \
}

#endif

#define EXPECT_EQ(_x_, _y_) EXPECT_OP(_x_, _y_, ==, "EQ")
#define EXPECT_NE(_x_, _y_) EXPECT_OP(_x_, _y_, !=, "NE")
#define EXPECT_GE(_x_, _y_) EXPECT_OP(_x_, _y_, >=, "GE")
#define EXPECT_LE(_x_, _y_) EXPECT_OP(_x_, _y_, <=, "LE")
#define EXPECT_GT(_x_, _y_) EXPECT_OP(_x_, _y_, >, "GT")
#define EXPECT_LT(_x_, _y_) EXPECT_OP(_x_, _y_, <, "LT")
