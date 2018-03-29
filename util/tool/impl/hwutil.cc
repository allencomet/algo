#include "../hwutil.h"


namespace toolHelp
{

CHwUtil::CHwUtil()
{
#ifdef WIN32
	m_hDllInstance = LoadLibrary("kernel32.dll");
	if (m_hDllInstance != NULL)
	{
		m_lpszGMStatusEx = (lpszGMStatusEx)GetProcAddress(m_hDllInstance, "GlobalMemoryStatusEx");
	}
	else
	{
		m_lpszGMStatusEx = NULL;
	}
#endif
}

CHwUtil::~CHwUtil()
{
#ifdef WIN32
	if (m_hDllInstance != NULL)
	{
		FreeLibrary(m_hDllInstance);
	}
#endif
}

#ifdef WIN32
#if _MSC_VER >= 1500
static bool getCPUIdelTmAndSysTm(double& dIdelTm, double& dSysTm)
{
	FILETIME  stIdelTm = {0};
	FILETIME  stSysTm = {0};
	
	//获取cpu空闲时间
	if(!GetSystemTimes(&stIdelTm, NULL, NULL))
	{
		return false;
	}
	
	//获取cpu系统时间
	GetSystemTimeAsFileTime(&stSysTm);
	
	dIdelTm = FT2D(stIdelTm);
	dSysTm  = FT2D(stSysTm);
	
	return true;
}
#endif
#endif

// get cpu usage
float	CHwUtil::getCpuUsage()
{
	float nCpuUsate = 0.0;
	
#ifdef WIN32
#if _MSC_VER >= 1500
	int    ProcessorNum = 0;
	double dLstIdleTm	= 0;
	double dLstSysTm	= 0;
	double dCurIdleTm	= 0;
	double dCurSysTm	= 0;

	SYSTEM_INFO sysinfo;
	GetSystemInfo(&sysinfo);
	ProcessorNum = sysinfo.dwNumberOfProcessors;

	if(getCPUIdelTmAndSysTm(dLstIdleTm, dLstSysTm))
	{
		Sleep(1000);

		if(getCPUIdelTmAndSysTm(dCurIdleTm, dCurSysTm))
		{
			double dIdelTmtemp	= dCurIdleTm - dLstIdleTm;
			double dSysTmtemp	= dCurSysTm	- dLstSysTm;
			double dCurTm		= dIdelTmtemp/dSysTmtemp;
			
			nCpuUsate = unsigned(100 - dCurTm / ProcessorNum * 100);
		}
	}
#else
	nCpuUsate = m_sCpuUsage.GetCpuUsage();
#endif
#else
	struct cpu_info cpu_info_old, cpu_info_new;
	memset(&cpu_info_new,0,sizeof(cpu_info));
	memset(&cpu_info_old,0,sizeof(cpu_info));

	ReadCpuInfo(&cpu_info_old);
	sleep(1);
	ReadCpuInfo(&cpu_info_new);
	
	long unsigned total_delta_time = (cpu_info_new.utime + cpu_info_new.ntime + cpu_info_new.stime + 
		cpu_info_new.itime + cpu_info_new.iowtime + cpu_info_new.irqtime + cpu_info_new.sirqtime + 
		cpu_info_new.stealtime) - 
		(cpu_info_old.utime + cpu_info_old.ntime + cpu_info_old.stime + cpu_info_old.itime + 
		cpu_info_old.iowtime + cpu_info_old.irqtime + cpu_info_old.sirqtime + 
		cpu_info_old.stealtime);
	
	nCpuUsate = (100 - (float)(cpu_info_new.itime - cpu_info_old.itime) * 100.0 / total_delta_time);
#endif
	
	return nCpuUsate;
}

// get mem usage
float	CHwUtil::getMemUsage()
{
	float nMemUsage = 0;

#ifdef WIN32
	if (m_lpszGMStatusEx != NULL)
	{
		MEMORYSTATUSEX	Meminfo;

		memset(&Meminfo, 0, sizeof(Meminfo));
		Meminfo.dwLength = sizeof(Meminfo);
		
		m_lpszGMStatusEx(&Meminfo);
		
		nMemUsage = Meminfo.dwMemoryLoad;
	}
#else
	struct mem_info mem_info;
	memset(&mem_info,0,sizeof(mem_info));
    ReadMemInfo(&mem_info);
	
	if(mem_info.mem_total > 0)
	{
		nMemUsage = (mem_info.mem_total - mem_info.mem_free - mem_info.buffers - mem_info.cached) * 100 / mem_info.mem_total;
	}
#endif
	
	return nMemUsage;
}

// get mem used
unsigned long CHwUtil::getMemUsed()
{
	unsigned long lMemUsed = 0;

	struct mem_info mem_info;
	memset(&mem_info,0,sizeof(mem_info));
    ReadMemInfo(&mem_info);

	if(mem_info.mem_total > 0)
	{
		lMemUsed = (mem_info.mem_total - mem_info.mem_free - mem_info.buffers - mem_info.cached) * 1024;
	}
	
	return lMemUsed;
}

// get mem size
#ifdef WIN32
ULONGLONG CHwUtil::getMemSize()
#else
uint64_t  CHwUtil::getMemSize()
#endif
{
#ifdef WIN32
	ULONGLONG	ullMemSize = 0;
#else
	uint64_t	ullMemSize = 0;
#endif

#ifdef WIN32
	if (m_lpszGMStatusEx != NULL)
	{
		MEMORYSTATUSEX	Meminfo;

		memset(&Meminfo, 0, sizeof(Meminfo));
		Meminfo.dwLength = sizeof(Meminfo);
		
		m_lpszGMStatusEx(&Meminfo);
		
		ullMemSize = Meminfo.ullTotalPhys;
	}
#else
	struct mem_info mem_info;
    ReadMemInfo(&mem_info);
	
	ullMemSize = (uint64_t)(mem_info.mem_total * 1024);
#endif

	return ullMemSize;
}

std::map<std::string,int64>& CHwUtil::GetDiskSize()
{
#ifdef WIN32
	return m_sWindowsHardDisk.GetDiskSize();
#else
	return m_sLinuxHardDisk.GetDiskSize();
#endif
}

std::map<std::string,int64>& CHwUtil::GetDiskFreeSize()
{
#ifdef WIN32
	return m_sWindowsHardDisk.GetDiskFreeSize();
#else
	return m_sLinuxHardDisk.GetDiskFreeSize();
#endif
}

std::map<std::string,int64>& CHwUtil::GetDiskUsedSize()
{
	return m_sLinuxHardDisk.GetDiskUsedSize();
}

}