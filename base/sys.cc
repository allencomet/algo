#include "sys.h"
#include "cclog/cclog.h"
#include "errno.h"

namespace sys {
namespace xx {

//将本地时间转换成字符串
std::string local_time::to_string(int64 sec, const char* format) {
    struct tm tm;
    ::gmtime_r(&sec, &tm);

    std::string s(32, '\0');
    size_t r = ::strftime(&s[0], 32, format, &tm);
    s.resize(r);

    return s;
}

int32 to_week(int32 year,int32 month,int32 day) {
    int w;    //星期
	int c;    //世纪-1 YYYY的头两位
	int y;    //年份   YYYY的后两位
	int m;    //月份 >=3 1月 2月看成上年的13月 14月
	int d=day;    //日
	if(month>=3) {
		c=year / 100;
		y=year % 100;
		m=month;
	} else {
		m=month+12;
		y=(year-1) % 100;
		c=(year-1) / 100;
	}
	w=y+y/4+c/4-2*c+(26*(m+1))/10+d-1;
	w=(w+700) %7;
	return w;
}

bool date_time::is_weekend(int year,int month,int day) {
    int nWeekDay = date_time::to_week(year,month,day);
	if(6 == nWeekDay || 7 == nWeekDay){
		return true;
	}else{
		return false;
	}
}

//注册信号回调
bool signal::set_handler(int sig, handler_t handler, int flag) {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sigemptyset(&sa.sa_mask);

    sa.sa_flags = flag;
    sa.sa_handler = handler;

    return sigaction(sig, &sa, NULL) != -1;
}

//获取信号回调
handler_t signal::get_handler(int sig) {
    struct sigaction sa;
    sigaction(sig, NULL, &sa);
    return sa.sa_handler;
}

//注册信号回调(自主维护一个回调列表,在信号触发后逐一调用)
void signal_handler::add_handler(int sig, 
#if __cplusplus >= 201103L
	std::function<void()> cb,
#else
	boost::function<void()> cb,
#endif
	int flag) {
    std::map<int, cbque>::iterator it = _map.find(sig);
    cbque& cbs = (it != _map.end()) ? it->second : (_map[sig] = cbque());//不存在则新建一个
    if (it != _map.end()) {
        cbs.push_back(cb);
        return;
    }

    handler_t oh = signal::get_handler(sig);
    if (oh != SIG_DFL && oh != SIG_IGN && oh != SIG_ERR) {
#if __cplusplus >= 201103L
		cbs.push_back(std::bind(oh, sig));//添加原信号处理回调
#else
		cbs.push_back(boost::bind(oh, sig));//添加原信号处理回调
#endif
        
    }

    signal::set_handler(sig, &signal_handler::on_signal, flag);
    cbs.push_back(cb);//追加最新注册的信号回调函数
}

//删除信号对应所有回调
void signal_handler::del_handler(int sig) {
    _map.erase(sig);//从信号处理列表中删除
    signal::reset(sig);//重置该信号为默认处理
}

//信号回调：处理信号(只处理一次，触发一次后所有回调逐一运行后删除)
void signal_handler::handle_signal(int sig) {
    std::map<int, cbque>::iterator it = _map.find(sig);
    if (it == _map.end()) return;

    cbque& cbs = it->second;

    while (!cbs.empty()) {
#if __cplusplus >= 201103L
		std::function<void()> cb = cbs.back();
#else
		boost::function<void()> cb = cbs.back();
#endif
        
        cbs.pop_back();
        cb();
    }
}

} // namespace xx

//睡眠按毫秒计算
void msleep(uint32 ms) {
    struct timespec ts;
    ts.tv_sec = ms / 1000;
    ts.tv_nsec = ms % 1000 * 1000000;

    while (nanosleep(&ts, &ts) == -1 && errno == EINTR);
}

//睡眠按微秒计算
void usleep(uint32 us) {
    struct timespec ts;
    ts.tv_sec = us / 1000000;
    ts.tv_nsec = us % 1000000 * 1000;

    while (nanosleep(&ts, &ts) == -1 && errno == EINTR);
}

} // namespace sys
