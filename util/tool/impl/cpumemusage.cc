#include "../cpumemusage.h"
#include <string.h>

namespace toolHelp
{

#ifndef WIN32

int ReadCpuInfo(struct cpu_info *p_cpu_info)
{
    FILE *fp = NULL;
    fp = fopen("/proc/stat", "r");
    if(!fp)
    {
        die("Could not open /proc/stat.\n");
    	//Applog(LOG_WARN,"打开[/proc/stat]文件失败,错误消息[%d:%s]", errno, strerror(errno) );
		return -1;
    }
    fscanf(fp, "cpu %lu %lu %lu %lu %lu %lu %lu %lu", &p_cpu_info->utime, &p_cpu_info->ntime,
		&p_cpu_info->stime, &p_cpu_info->itime, &p_cpu_info->iowtime,
		&p_cpu_info->irqtime, &p_cpu_info->sirqtime, &p_cpu_info->stealtime);
    fclose(fp);

    return 0;
}

int ReadMemInfo(struct mem_info *p_mem_info)
{
	FILE *fp = NULL;    
	char szTemp[32] = {0};    
	long unsigned lTemp = 0;    
	fp = fopen("/proc/meminfo", "r");    
	if(!fp)    
	{        
		die("Could not open /proc/meminfo.\n");
		//Applog(LOG_WARN,"打开[/proc/meminfo]文件失败,错误消息[%d:%s]", errno, strerror(errno) );
		return -1;
	}    

	fscanf(fp, "%*s%lu%*s\n%*s%lu%*s\n%s%lu%*s\n", &p_mem_info->mem_total,
				&p_mem_info->mem_free, szTemp, &lTemp);    
	if(strncmp("MemAvailable", szTemp, strlen("MemAvailable")))    
	{        
		//mem_available = -1表示系统内核版本小于3.14，不存在MemAvailable字段        
		p_mem_info->mem_available = -1;        
		p_mem_info->buffers = lTemp;        
		fscanf(fp, "%*s%lu%*s\n", &p_mem_info->cached);    
	}    
	else    
	{        
		p_mem_info->mem_available = lTemp;        
		fscanf(fp, "%*s%lu%*s\n%*s%lu%*s\n", &p_mem_info->buffers, &p_mem_info->cached);    
	}    
	fclose(fp);
	
	return 0;
}


}
#endif
