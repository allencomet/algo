#pragma once

#include <stdio.h>

#if __cplusplus >= 201103L

#include <functional>

#else

#include <boost/function.hpp>

#endif


namespace cclog {
namespace xx {

//错误处理句柄（虚基类）
class FailureHandler {
  public:
	  FailureHandler() {};
	  virtual ~FailureHandler() {};

    virtual void set_fd(FILE* file) = 0;
#if __cplusplus >= 201103L
	virtual void set_handler(std::function<void()> cb) = 0;
#else
	virtual void set_handler(boost::function<void()> cb) = 0;
#endif

  private:
#if __cplusplus >= 201103L
	  FailureHandler(const FailureHandler &) = delete;
	  FailureHandler& operator=(const FailureHandler &) = delete;
#else
	  FailureHandler(const FailureHandler &);
	  FailureHandler& operator=(const FailureHandler &);
#endif
	  
};

FailureHandler* NewFailureHandler();

}  // namespace xx
}  // namespace cclog
