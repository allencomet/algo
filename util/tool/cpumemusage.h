#ifndef _CPU_MEM_USAGE_H
#define _CPU_MEM_USAGE_H



#ifndef WIN32

#include <cstdio>
#include <cstdlib>
#include <unistd.h>

namespace toolHelp
{
#define die(...) { fprintf(stderr, __VA_ARGS__); exit(EXIT_FAILURE); }

struct cpu_info {
    long unsigned utime, ntime, stime, itime;
    long unsigned iowtime, irqtime, sirqtime;
    long unsigned stealtime;
};

struct mem_info {
    long unsigned mem_total, mem_free, mem_available, buffers, cached;
};

int ReadCpuInfo(struct cpu_info *p_cpu_info);
int ReadMemInfo(struct mem_info *p_mem_info);

}

#endif




#endif

