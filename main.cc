#include "util/util.h"

int64 mktime(const std::string& t) {
    struct tm tm;
    memset(&tm, 0, sizeof(struct tm));
    strptime(t.c_str(), "%Y-%m-%d %H:%M:%S", &tm);

    return ::mktime(&tm);
}


inline double random_factor(int64 v) {
    return 0.01 * (v % 7);
}

int32 go(int32 n) {
    if (n < 180) return n;

    int64 us = sys::local_time.us();
    int64 sec = us / 1000000;
    int64 min = sec / 60 % 60;
    int64 h = sec / 3600 % 24;

    // get a random factor from 0.00~0.06
    double r = random_factor(us);
    double m = (min - min % 10) / 60.0;

    double rate = 0;

    if (h < 16) {        /* [0.6, 0.76) + 0.00~0.06 */
        rate = 0.6 + 0.01 * (h + m) + r;
    } else if (h < 22) { /* [0.77, 0.89) + -0.01~0.02 */
        rate = 0.75 + (h - 15 + m) * 0.02 + (r - 0.02) / 2;
    } else {             /* [0.93, 0.99) + -0.02~0.01 */
        rate = 0.9 + (h - 21 + m) * 0.03 + (r - 0.04) / 2;
    }

    if (rate < 0 || rate > 1) {
        TLOG("predict") << "rate error: " << rate;
        return n;
    }

    COUT << rate;

    return static_cast<int32>(n / rate);
}

DEF_test(main) {
    int32 x = 3000;
    COUT << go(150);
    COUT << go(x);
    COUT << go(x);
    COUT << go(x);
    COUT << go(x);
    COUT << go(x);
    COUT << go(x);
    COUT << go(x);
    COUT << go(x);
}

int main(int argc,char **argv){
    /*FLG_main = true;
	char *p = NULL;
	char *p2 = p;
	p2 = new char[1];
	if (NULL == p) {
		COUT << "p is NULL";
	}else {
		COUT << "p is not NULL";
	}

	if (NULL == p2) {
		COUT << "p2 is NULL";
	}else {
		COUT << "p2 is not NULL";
	}*/

	//os::init_daemon();
	//os::daemon();

    if(!os::run_single_instance()) return 1;

	ccflag::init_ccflag(argc, argv);
	cclog::init_cclog(argv[0]);

	cctest::init_cctest(argc, argv);
	cctest::run_tests();

	int i = 0;
	std::string str("hello");

	LOG << "hello" << str << i << (void*)&i;
	LOG_IF(1 + 1 == 2) << "1 + 1 == 2";
	WLOG << "hello warning";
	ELOG << "hello error";
	CHECK_EQ(1 + 1, 2);

	TLOG("hello") << "log to specific file";

	//for (int i = 0; i < 100000; ++i) {
	//	DLOG("xxx") << "log only --dlog_on=true";
	//	COUT << i;
	//}
	
    return 0;
}

