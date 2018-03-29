#ifndef  GETHARDDISK_H
#define	 GETHARDDISK_H

#include "../base/base.h"
#include <sys/vfs.h>
#include <stdio.h>
#include <sys/types.h>


namespace toolHelp
{

#ifndef WIN32
//获取硬盘大小
class HardDisk
{
public:
	HardDisk();
	~HardDisk();


	//读取各个磁盘的信息
	void ReadDiskInfo();

	//读取/etc/mtab获取分区的信息
	void ReadMtab(const char *strFileName);

	//获取硬盘总的容量
	std::map<std::string,int64>& GetDiskSize(); 


	//获取硬盘剩余的容量
	std::map<std::string,int64>& GetDiskFreeSize();


	//获取硬盘已使用的容量
	std::map<std::string,int64>& GetDiskUsedSize();
public:
	struct statfs  diskInfo;
	int64 m_TotalSize;  //硬盘的总的容量 单位为:/MB
	int64 m_FreeSize;	//硬盘剩余的容量 单位为:/MB
	int64 m_UsedSize ; //硬盘已使用的容量
	std::list<std::string>	m_ListName;
	std::map<std::string,int64>		m_mapTotalSize;
	std::map<std::string,int64>     m_mapFreeSize ;
	std::map<std::string,int64>     m_mapUsedSize ;
};
#endif


#ifdef  WIN32
class WindowsHardDisk
{
public :
	WindowsHardDisk();
	~WindowsHardDisk();

	std::map<std::string,int64>& GetDiskSize();
	std::map<std::string,int64>& GetDiskFreeSize();
	void GetDiskInfo();
public:
	int64 m_TotalSize;  //硬盘的总的容量 单位为:/MB
	int64 m_FreeSize;	//硬盘剩余的容量 单位为:/MB
	int64 m_UsedSize;

	std::map<std::string,int64>	   m_mapTotalSize;
	std::map<std::string,int64>    m_mapFreeSize ;
};
#endif


}

#endif
