#include "cctimer.h"

namespace cctimer{

void Timer::Expire(){
    if (expired_){
        return;
    }

    if (try_to_expire_){
        //std::cout << "timer is trying to expire, please wait..." << std::endl;
        return;
    }
    try_to_expire_ = true;
    {
        std::unique_lock<std::mutex> locker(mutex_);
        expired_cond_.wait(locker, [this]{return expired_ == true; });
        if (expired_ == true){
            //std::cout << "timer expired!" << std::endl;
            try_to_expire_ = false;
        }
    }
}

}//namespace cctimer