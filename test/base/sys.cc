#include "../../util/util.h"

namespace test{

DEF_test(test_sys){
    DEF_case(utc) {
        EXPECT_EQ(true, sys::utc.is_today(sys::utc.sec()));
        EXPECT_EQ(false, sys::utc.is_today(sys::utc.sec()+24*3600));
        COUT << "sys::utc.day[since EPOCH]: " << sys::utc.day();
        COUT << "sys::utc.hour[since EPOCH]: " << sys::utc.hour();
        COUT << "sys::utc.sec[since EPOCH]: " << sys::utc.sec();
        COUT << "sys::utc.ms[since EPOCH]: " << sys::utc.ms();
        COUT << "sys::utc.us[since EPOCH]: " << sys::utc.us();
    }
    
    DEF_case(local_time) {
        EXPECT_EQ(true, sys::local_time.is_today(sys::local_time.sec()));
        EXPECT_EQ(false, sys::local_time.is_today(sys::local_time.sec()+24*3600));
        COUT << "sys::local_time.to_string[%Y-%m-%d %H:%M:%S]: " << sys::local_time.to_string();
        COUT << "sys::local_time.to_string[%H:%M:%S]: " << sys::local_time.to_string("%H:%M:%S");
        COUT << "sys::local_time.day: " << sys::local_time.day();
        COUT << "sys::local_time.hour: " << sys::local_time.hour();
        COUT << "sys::local_time.sec: " << sys::local_time.sec();
        COUT << "sys::local_time.ms: " << sys::local_time.ms();
        COUT << "sys::local_time.us: " << sys::local_time.us();
    }

    DEF_case(date) {
        {
            sys::xx::date date(2018,5,16);
            EXPECT_EQ(3, date.tm_wday());
        }
       
        {
            sys::xx::date date(2018,1,3);
            EXPECT_EQ(3, date.tm_wday());
            EXPECT_EQ(3, date.tm_yday());
        }
    }

    DEF_case(date_time) {
        COUT << "sys::date_time.week: " << sys::date_time.week();
        COUT << "sys::date_time.week_r: " << sys::date_time.week_r();
        COUT << "sys::date_time.day: " << sys::date_time.day();
        COUT << "sys::date_time.day_r: " << sys::date_time.day_r();
        COUT << "sys::date_time.month: " << sys::date_time.month();
        COUT << "sys::date_time.month_r: " << sys::date_time.month_r();
        COUT << "sys::date_time.year: " << sys::date_time.year();
        COUT << "sys::date_time.year_r: " << sys::date_time.year_r();
    }
}

//计算指定日期在多少时间之后

}//namespace test