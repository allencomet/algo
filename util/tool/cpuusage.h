#ifndef _CPU_USAGE_H
#define _CPU_USAGE_H



#ifdef WIN32

#include <WINDOWS.H>
#include <ATLBASE.H>


namespace toolHelp
{
class CCpuUsage  
{
public:
	CCpuUsage();
	virtual ~CCpuUsage();

	// Methods
	int GetCpuUsage();
	int GetCpuUsage(LPCTSTR pProcessName);
	
	BOOL EnablePerformaceCounters(BOOL bEnable = TRUE);
	
	// Attributes
private:
	bool			m_bFirstTime;
	LONGLONG		m_lnOldValue ;
	LARGE_INTEGER	m_OldPerfTime100nSec;
};

}

#endif




#endif
