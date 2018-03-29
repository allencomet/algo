#ifndef  GETHARDDISK_H
#define	 GETHARDDISK_H

#include "../base/base.h"
#include <sys/vfs.h>
#include <stdio.h>
#include <sys/types.h>


namespace toolHelp
{

#ifndef WIN32
//��ȡӲ�̴�С
class HardDisk
{
public:
	HardDisk();
	~HardDisk();


	//��ȡ�������̵���Ϣ
	void ReadDiskInfo();

	//��ȡ/etc/mtab��ȡ��������Ϣ
	void ReadMtab(const char *strFileName);

	//��ȡӲ���ܵ�����
	std::map<std::string,int64>& GetDiskSize(); 


	//��ȡӲ��ʣ�������
	std::map<std::string,int64>& GetDiskFreeSize();


	//��ȡӲ����ʹ�õ�����
	std::map<std::string,int64>& GetDiskUsedSize();
public:
	struct statfs  diskInfo;
	int64 m_TotalSize;  //Ӳ�̵��ܵ����� ��λΪ:/MB
	int64 m_FreeSize;	//Ӳ��ʣ������� ��λΪ:/MB
	int64 m_UsedSize ; //Ӳ����ʹ�õ�����
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
	int64 m_TotalSize;  //Ӳ�̵��ܵ����� ��λΪ:/MB
	int64 m_FreeSize;	//Ӳ��ʣ������� ��λΪ:/MB
	int64 m_UsedSize;

	std::map<std::string,int64>	   m_mapTotalSize;
	std::map<std::string,int64>    m_mapFreeSize ;
};
#endif


}

#endif
