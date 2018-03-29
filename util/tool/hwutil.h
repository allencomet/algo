#pragma once

#include "../base/base.h"



#ifdef WIN32
#include "cpuusage.h"
#else
#include "cpumemusage.h"
#endif

#ifdef WIN32
#include <windows.h>
#include "getharddisk.h"
#else
#include <stdint.h>
#include "getharddisk.h"
#endif

#ifdef WIN32
#if _MSC_VER >= 1500
#define FT2D(x) ((double)((x).dwHighDateTime) * 4.294967296E9 + (double)((x).dwLowDateTime))
#else
typedef BOOL (WINAPI * lpszGMStatusEx)(LPMEMORYSTATUSEX lpBuffer);
#endif
#endif

namespace toolHelp
{

class CHwUtil  
{
public:
	CHwUtil();
	virtual ~CHwUtil();

public:
	// get cpu usage
	float	getCpuUsage();
	
	// get mem usage
	float	getMemUsage();	

	//get mem used
	unsigned long getMemUsed();
	
	// get mem size
#ifdef WIN32
	ULONGLONG	getMemSize();
#else
	uint64_t	getMemSize();
#endif

	std::map<std::string,int64>&  GetDiskSize();
	std::map<std::string,int64>&  GetDiskFreeSize();
	std::map<std::string,int64>&  GetDiskUsedSize();
private:
#ifdef WIN32
	CCpuUsage		m_sCpuUsage;
	HINSTANCE		m_hDllInstance;
	lpszGMStatusEx	m_lpszGMStatusEx;
#endif

#ifdef WIN32
	WindowsHardDisk	m_sWindowsHardDisk;

#else

	HardDisk		m_sLinuxHardDisk;

#endif
};

}
