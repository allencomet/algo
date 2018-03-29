#ifndef TIME_H_
#define TIME_H_

#include "../base/base.h"
#include "../base/data_types.h"

namespace toolHelp
{

class CTime
{
public:        
		//返回年份格式的字串(如:2012)
		static std::string GetShortYearStr();
        //返回短时间格式的字串(如:2012-01-01)
        static std::string GetShortTimeStr();
		//返回短时间格式的字串(如:20120101)
		static std::string GetCurDateStr();
        //返回长时间格式的字串(如:2012-01-01 01:01:01)
        static std::string GetLongTimeStr();
		//获取当前时间(如:01:01:01)
		static std::string GetCurShortTimeStr();
        //返回当前时间(从1970.01.01 00:00:00以来的秒数)
        static long GetCurTime();
        //返回当前时间(从1970.01.01 00:00:00以来的毫秒数) 
        static long GetTotalMilliseconds();
        //返回当前时间(从1970.01.01 00:00:00以来的微秒数) 
        static long GetTotalMicroseconds();
		//返回当前分钟数(如:01:01分钟数为1*60+1)
		static int	GetCurIntMinute();

		//传入数字，获取前一天
		static int GetYesterday(int nDate);
		//传入数字，获取后一天
		static int GetTomorrow(int nDate);



		static int GetCurWeek();
		static int GetCurDay();
		static int GetCurMonth();
		static int GetCurYear();
		static bool IsWeekend();	//判断是否为周末

		int GetWeek(int year,int month,int day);
		bool IsWeekend(int year,int month,int day);
protected:
       CTime() {}
       ~CTime() { }
};

}



#endif /* TIME_H_ */
