//#include "stdafx.h"
#include "../time.h"

namespace toolHelp
{

static void split_token(const std::string& str, std::vector<std::string>& list, const char *token){
	list.clear();
	const char* begin = str.c_str();
	const char* end;

	while (*begin){
		while (*begin && strchr(token, *begin)){
			++begin;
		}

		end = begin;
		while (*end && strchr(token, *end) == NULL){
			++end;
		}

		if (*begin){
			list.push_back(std::string(begin, end - begin));
		}

		begin = end;
	}
}


//������ݸ�ʽ���ִ�(��:2012)
std::string CTime::GetShortYearStr(){
	struct timeval tv;
	char buffer[20] = {0};

	gettimeofday(&tv,NULL);
	strftime(buffer,sizeof(buffer),"%Y",localtime(&tv.tv_sec));

	return std::string(buffer);
}

////////////////////////////////////////////////////////////////////////////////
// 
// ��������:    GetLongTimeStr
// 
// ����˵��:    ���ض�ʱ���ʽ���ִ�(��:2012-01-01)
// 
// ����˵��:    
// 
// �� �� ֵ:   
// 
// ��Ҫ˵��:    
// 
////////////////////////////////////////////////////////////////////////////////
std::string  CTime::GetShortTimeStr(){
    struct timeval tv;
    char buffer[20] = {0};

    gettimeofday(&tv,NULL);
    strftime(buffer,sizeof(buffer),"%Y-%m-%d",localtime(&tv.tv_sec));

    return std::string(buffer);
}


//���ض�ʱ���ʽ���ִ�(��:20120101)
std::string CTime::GetCurDateStr(){
	struct timeval tv;
	char buffer[20] = {0};

	gettimeofday(&tv,NULL);
	strftime(buffer,sizeof(buffer),"%Y%m%d",localtime(&tv.tv_sec));

	return std::string(buffer);
}

////////////////////////////////////////////////////////////////////////////////
// 
// ��������:    GetLongTimeStr
// 
// ����˵��:    ���س�ʱ���ʽ���ִ�(��:2012-01-01 01:01:01)
// 
// ����˵��:    
// 
// �� �� ֵ:   
// 
// ��Ҫ˵��:    
// 
////////////////////////////////////////////////////////////////////////////////
std::string  CTime::GetLongTimeStr(){
    struct timeval tv;
    char buffer[20] = {0};

    gettimeofday(&tv,NULL);
    strftime(buffer,sizeof(buffer),"%Y-%m-%d %T",localtime(&tv.tv_sec));

    return std::string(buffer);
}

//��ȡ��ǰʱ��(��:01:01:01)
std::string CTime::GetCurShortTimeStr(){
	struct timeval tv;
    char buffer[20] = {0};

    gettimeofday(&tv,NULL);
    strftime(buffer,sizeof(buffer),"%T",localtime(&tv.tv_sec));

    return std::string(buffer);
}

////////////////////////////////////////////////////////////////////////////////
// 
// ��������:    GetCurTime
// 
// ����˵��:    ���ش�1970.01.01 00:00:00����������
// 
// ����˵��:    
// 
// �� �� ֵ:   
// 
// ��Ҫ˵��:    
// 
////////////////////////////////////////////////////////////////////////////////
long  CTime::GetCurTime(){
    return time(NULL);
}

////////////////////////////////////////////////////////////////////////////////
// 
// ��������:    GetCurMillisecond
// 
// ����˵��:    ���ش�1970.01.01 00:00:00�����ĺ�����
// 
// ����˵��:    
// 
// �� �� ֵ:   
// 
// ��Ҫ˵��:    
// 
////////////////////////////////////////////////////////////////////////////////
long  CTime::GetTotalMilliseconds(){
    timeval tv_cur;
    gettimeofday(&tv_cur, NULL);
    return (1000000 * tv_cur.tv_sec + tv_cur.tv_usec) / 1000;
}


////////////////////////////////////////////////////////////////////////////////
// 
// ��������:    GetTotalMicroseconds
// 
// ����˵��:    ���ش�1970.01.01 00:00:00������΢����
// 
// ����˵��:    
// 
// �� �� ֵ:   
// 
// ��Ҫ˵��:    
// 
////////////////////////////////////////////////////////////////////////////////
long  CTime::GetTotalMicroseconds(){
    timeval tv_cur;
    gettimeofday(&tv_cur, NULL);
    return 1000000 * tv_cur.tv_sec + tv_cur.tv_usec;
}

////////////////////////////////////////////////////////////////////////////////
// 
// ��������:    GetCurIntMinute
// 
// ����˵��:    ���ص�ǰ������(��:01:01������Ϊ1*60+1)
// 
// ����˵��:    
// 
// �� �� ֵ:   
// 
// ��Ҫ˵��:    
// 
////////////////////////////////////////////////////////////////////////////////
int CTime::GetCurIntMinute(){
	struct timeval tv;
    char buffer[20] = {0};

    gettimeofday(&tv,NULL);
    strftime(buffer,sizeof(buffer),"%H:%M",localtime(&tv.tv_sec));

	std::string strResult = buffer;
	
	std::vector<std::string> vectorTime;
	vectorTime.clear();

	split_token(strResult,vectorTime,":");

	int nValue = 0;
	
	switch(vectorTime.size()){
	case 1:
		nValue = atoi(vectorTime[0].c_str())*60;
		break;
	case 2:
    	nValue = atoi(vectorTime[0].c_str())*60+atoi(vectorTime[1].c_str());
		break;
	}
    return nValue;
}

int CTime::GetYesterday( int nDate ){
	int nYear = nDate/10000;
	int nMonth = nDate/100%100;
	int nday = nDate%100;
	int nHour = 0;
	int nMin = 0;
	int nSecond = 0;
	struct tm ptm1;

	ptm1.tm_year = nYear-1900;
	ptm1.tm_mon = nMonth-1;
	ptm1.tm_mday = nday ;
	ptm1.tm_hour = nHour;
	ptm1.tm_min = nMin;
	ptm1.tm_sec = nSecond;

	time_t lt =  mktime(&ptm1);
	lt -= 24*60*60;
	struct tm* ptm2 = localtime(&lt);
	nYear = ptm2->tm_year+1900;
	nMonth = ptm2->tm_mon+1;
	nday = ptm2->tm_mday;
	return nYear*10000+nMonth*100+nday;
}


//�������֣���ȡ��һ��
int CTime::GetTomorrow(int nDate){
	int nYear = nDate/10000;
	int nMonth = nDate/100%100;
	int nday = nDate%100;
	int nHour = 0;
	int nMin = 0;
	int nSecond = 0;
	struct tm ptm1;

	ptm1.tm_year = nYear-1900;
	ptm1.tm_mon = nMonth-1;
	ptm1.tm_mday = nday ;
	ptm1.tm_hour = nHour;
	ptm1.tm_min = nMin;
	ptm1.tm_sec = nSecond;

	time_t lt =  mktime(&ptm1);
	lt += 24*60*60;
	struct tm* ptm2 = localtime(&lt);
	nYear = ptm2->tm_year+1900;
	nMonth = ptm2->tm_mon+1;
	nday = ptm2->tm_mday;
	return nYear*10000+nMonth*100+nday;
}

int CTime::GetCurYear(){
	int year;
	time_t t;
	struct tm * timeinfo;
	time(&t);
	timeinfo = localtime(&t);
	year = timeinfo->tm_year + 1900;

	return year;
}

int CTime::GetCurMonth(){
	int month;
	time_t t;
	struct tm * timeinfo;
	time(&t);
	timeinfo = localtime(&t);
	month = timeinfo->tm_mon + 1;

	return month;
}

int CTime::GetCurDay(){
	int day;
	time_t t;
	struct tm * timeinfo;
	time(&t);
	timeinfo = localtime(&t);
	day = timeinfo->tm_mday;

	return day;
}

int CTime::GetCurWeek(){
	int wday;
	time_t t;
	struct tm * timeinfo;
	time(&t);
	timeinfo = localtime(&t);
	wday = timeinfo->tm_wday;

	return wday;
}

//�ж��Ƿ�Ϊ��ĩ
bool CTime::IsWeekend(){
	int nWeek = GetCurWeek();
	if (6 == nWeek || 0 == nWeek){
		return true;
	}else{
		return false;
	}
}

//����̩�չ�ʽ�������ڼ�
//w=y+[y/4]+[c/4]-2c+[26(m+1)/10]+d-1 
int CTime::GetWeek(int year,int month,int day){
	int w;    //����
	int c;    //����-1 YYYY��ͷ��λ
	int y;    //���   YYYY�ĺ���λ
	int m;    //�·� >=3 1�� 2�¿��������13�� 14��
	int d=day;    //��
	if(month>=3) {
		c=year / 100;
		y=year % 100;
		m=month;
	}else{
		m=month+12;
		y=(year-1) % 100;
		c=(year-1) / 100;
	}
	w=y+y/4+c/4-2*c+(26*(m+1))/10+d-1;
	w=(w+700) %7;
	return w;
}

//�ж��Ƿ�Ϊ��ĩ
bool CTime::IsWeekend(int year,int month,int day){
	int nWeekDay = GetWeek(year,month,day);
	if(6 == nWeekDay || 7 == nWeekDay){
		return true;
	}else{
		return false;
	}
}

}