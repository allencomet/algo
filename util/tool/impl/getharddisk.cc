#include "../getharddisk.h"
//#include "../../../global.h"

namespace toolHelp
{

#ifndef WIN32

HardDisk::HardDisk()
{
	m_mapTotalSize.clear();
	m_mapFreeSize.clear();
	m_mapUsedSize.clear();

	ReadMtab("/etc/mtab");
	ReadDiskInfo();
}


HardDisk::~HardDisk()
{
	m_mapTotalSize.clear();
	m_mapFreeSize.clear();
	m_mapUsedSize.clear();
}

//读取各个磁盘的信息
void HardDisk::ReadDiskInfo()
{
	std::list<std::string>::iterator pIter;
	pIter = m_ListName.begin();
	while(pIter != m_ListName.end())
	{
		m_TotalSize = 0;
		m_FreeSize = 0;
		memset(&diskInfo,0,sizeof(diskInfo));
		statfs(pIter->c_str(),&diskInfo);
		m_TotalSize = (diskInfo.f_bsize * diskInfo.f_blocks ) /1024/1024;    //经过优化的传输块大小*文件系统数据块总数
		m_FreeSize = (diskInfo.f_bfree * diskInfo.f_bsize)/1024/1024; //已用块数 * 经过优化的传输块大小
		m_UsedSize = m_TotalSize - m_FreeSize ;

		m_mapTotalSize.insert(std::make_pair(pIter->c_str(),m_TotalSize));
		m_mapFreeSize.insert(std::make_pair(pIter->c_str(),m_FreeSize));
		m_mapUsedSize.insert(std::make_pair(pIter->c_str(),m_UsedSize));
		pIter ++;
	}
}


//读取/etc/mtab获取分区的信息
void HardDisk::ReadMtab(const char *strFileName)
{
	FILE *pFile;
	char szLine[1024] = {0};
	char keyBuffer[256] = {0};
	char valueBuffer[1024] = {0};    

	if((pFile = fopen(strFileName,"r")) == NULL)		return;

	while(fgets(szLine,1024,pFile) != NULL)
	{
		//TrimAll(szLine);
		str::strip(szLine);
		if(strlen(szLine) ==0)   continue;//是空行，跳过
		else  //是具体的item的值
		{
			memset(keyBuffer,0,sizeof(keyBuffer));
			memset(valueBuffer,0,sizeof(valueBuffer));

			char *pTemp;
			if((pTemp = strstr(szLine," /")) != NULL)
			{
				//strncpy(keyBuffer,szLine,pTemp-szLine);
				strcpy(valueBuffer,pTemp+1);
				char *pTemp1 ;
				if((pTemp1 = strstr(valueBuffer," ")) != NULL)
				{
					strncpy(keyBuffer,valueBuffer,pTemp1-valueBuffer);
				}
				//TrimRight(keyBuffer);
				//TrimLeft(valueBuffer);
				str::rstrip_nc(keyBuffer);
				str::lstrip_nc(valueBuffer);

				m_ListName.push_back(keyBuffer);

			}
		}
	}

	if(pFile !=NULL)		fclose(pFile);
}

//获取硬盘总的容量
std::map<std::string,int64>& HardDisk::GetDiskSize()
{
	return m_mapTotalSize;
} 

//获取硬盘剩余的容量
std::map<std::string,int64>& HardDisk::GetDiskFreeSize()
{
	return m_mapFreeSize;
}

//获取硬盘已使用的容量
std::map<std::string,int64>& HardDisk::GetDiskUsedSize()
{
	return m_mapUsedSize;
}

#endif



#ifdef  WIN32

WindowsHardDisk::WindowsHardDisk()
{
	m_mapTotalSize.clear();
	m_mapFreeSize.clear();

	GetDiskInfo();
}


WindowsHardDisk::~WindowsHardDisk()
{
	m_mapTotalSize.clear();
	m_mapFreeSize.clear();
}

std::map<std::string,int64>& WindowsHardDisk::GetDiskSize()
{
	return m_mapTotalSize;
}

std::map<std::string,int64>& WindowsHardDisk::GetDiskFreeSize()
{
	return m_mapFreeSize;
}


void WindowsHardDisk::GetDiskInfo()
{
		int DiskCount = 0;
		//获取系统中逻辑驱动器的数量，函数返回的是一个32位无符号整型数据
		DWORD DiskInfo = GetLogicalDrives();

		//查看每一位数据是否为1，如果为1则磁盘为真,如果为0则磁盘不存在。
		while(DiskInfo)
		{
			if(DiskInfo & 1)
			{
				++DiskCount;
			}
			DiskInfo = DiskInfo >> 1;
		}

		int DSLength = GetLogicalDriveStrings(0,NULL);

		char* DStr = new char[DSLength];

		 //通过GetLogicalDriveStrings将字符串信息复制到堆区数组中,其中保存了所有驱动器的信息。
		GetLogicalDriveStrings(DSLength,(LPTSTR)DStr);

		int si = 0;
		bool bResult;
		unsigned _int64 i64FreeBytesToCaller;  
		unsigned _int64 i64TotalBytes;  
		unsigned _int64 i64FreeBytes; 

		for(int i=0;i<DSLength/4;++i)   
		{  
			char dir[4]={0};
			memcpy(dir,&DStr[si],3);
			/*string DirName="";
			DirName += dir[0];
			DirName += "盘";*/

			//GetDiskFreeSpaceEx函数，可以获取驱动器磁盘的空间状态,函数返回的是个BOOL类型数据  
			bResult = GetDiskFreeSpaceEx (  
				  dir,  
				  (PULARGE_INTEGER)&i64FreeBytesToCaller,  
				  (PULARGE_INTEGER)&i64TotalBytes,  
				  (PULARGE_INTEGER)&i64FreeBytes);  
			 

			if(bResult)//通过返回的BOOL数据判断驱动器是否在工作状态 ，在工作状态则放入Map中 
			{  
				  m_TotalSize = (int64)(i64TotalBytes/1024/1024);//磁盘总容量 ，以M为单位  
				  m_FreeSize =  (int64)(i64FreeBytesToCaller/1024/1024);//磁盘剩余空间 ，以M为单位
				  m_UsedSize = m_TotalSize - m_FreeSize;

				  //将相应的信息放入Map中
				  m_mapTotalSize.insert(std::make_pair(dir,m_TotalSize));
				  m_mapFreeSize.insert(std::make_pair(dir,m_UsedSize));
				  
			}  
		
			si+=4; 
		}
		delete []DStr;
		DStr =NULL;
}

#endif


}