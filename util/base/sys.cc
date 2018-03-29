#include "sys.h"
#include "cclog/cclog.h"
#include "errno.h"

namespace sys {
namespace xx {

//������ʱ��ת�����ַ���
std::string local_time::to_string(int64 sec, const char* format) {
    struct tm tm;
    ::gmtime_r(&sec, &tm);

    std::string s(32, '\0');
    size_t r = ::strftime(&s[0], 32, format, &tm);
    s.resize(r);

    return s;
}

//ע���źŻص�
bool signal::set_handler(int sig, handler_t handler, int flag) {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sigemptyset(&sa.sa_mask);

    sa.sa_flags = flag;
    sa.sa_handler = handler;

    return sigaction(sig, &sa, NULL) != -1;
}

//��ȡ�źŻص�
handler_t signal::get_handler(int sig) {
    struct sigaction sa;
    sigaction(sig, NULL, &sa);
    return sa.sa_handler;
}

//ע���źŻص�(����ά��һ���ص��б�,���źŴ�������һ����)
void signal_handler::add_handler(int sig, boost::function<void()> cb, int flag) {
    std::map<int, cbque>::iterator it = _map.find(sig);
    cbque& cbs = (it != _map.end()) ? it->second : (_map[sig] = cbque());//���������½�һ��
    if (it != _map.end()) {
        cbs.push_back(cb);
        return;
    }

    handler_t oh = signal::get_handler(sig);
    if (oh != SIG_DFL && oh != SIG_IGN && oh != SIG_ERR) {
        cbs.push_back(boost::bind(oh, sig));//���ԭ�źŴ���ص�
    }

    signal::set_handler(sig, &signal_handler::on_signal, flag);
    cbs.push_back(cb);//׷������ע����źŻص�����
}

//ɾ���źŶ�Ӧ���лص�
void signal_handler::del_handler(int sig) {
    _map.erase(sig);//���źŴ����б���ɾ��
    signal::reset(sig);//���ø��ź�ΪĬ�ϴ���
}

//�źŻص��������ź�(ֻ����һ�Σ�����һ�κ����лص���һ���к�ɾ��)
void signal_handler::handle_signal(int sig) {
    std::map<int, cbque>::iterator it = _map.find(sig);
    if (it == _map.end()) return;

    cbque& cbs = it->second;

    while (!cbs.empty()) {
        boost::function<void()> cb = cbs.back();
        cbs.pop_back();
        cb();
    }
}

} // namespace xx

//˯�߰��������
void msleep(uint32 ms) {
    struct timespec ts;
    ts.tv_sec = ms / 1000;
    ts.tv_nsec = ms % 1000 * 1000000;

    while (nanosleep(&ts, &ts) == -1 && errno == EINTR);
}

//˯�߰�΢�����
void usleep(uint32 us) {
    struct timespec ts;
    ts.tv_sec = us / 1000000;
    ts.tv_nsec = us % 1000000 * 1000;

    while (nanosleep(&ts, &ts) == -1 && errno == EINTR);
}

} // namespace sys
