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

//��ȡ�������̵���Ϣ
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
		m_TotalSize = (diskInfo.f_bsize * diskInfo.f_blocks ) /1024/1024;    //�����Ż��Ĵ�����С*�ļ�ϵͳ���ݿ�����
		m_FreeSize = (diskInfo.f_bfree * diskInfo.f_bsize)/1024/1024; //���ÿ��� * �����Ż��Ĵ�����С
		m_UsedSize = m_TotalSize - m_FreeSize ;

		m_mapTotalSize.insert(std::make_pair(pIter->c_str(),m_TotalSize));
		m_mapFreeSize.insert(std::make_pair(pIter->c_str(),m_FreeSize));
		m_mapUsedSize.insert(std::make_pair(pIter->c_str(),m_UsedSize));
		pIter ++;
	}
}


//��ȡ/etc/mtab��ȡ��������Ϣ
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
		if(strlen(szLine) ==0)   continue;//�ǿ��У�����
		else  //�Ǿ����item��ֵ
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

//��ȡӲ���ܵ�����
std::map<std::string,int64>& HardDisk::GetDiskSize()
{
	return m_mapTotalSize;
} 

//��ȡӲ��ʣ�������
std::map<std::string,int64>& HardDisk::GetDiskFreeSize()
{
	return m_mapFreeSize;
}

//��ȡӲ����ʹ�õ�����
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
		//��ȡϵͳ���߼����������������������ص���һ��32λ�޷�����������
		DWORD DiskInfo = GetLogicalDrives();

		//�鿴ÿһλ�����Ƿ�Ϊ1�����Ϊ1�����Ϊ��,���Ϊ0����̲����ڡ�
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

		 //ͨ��GetLogicalDriveStrings���ַ�����Ϣ���Ƶ�����������,���б�������������������Ϣ��
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
			DirName += "��";*/

			//GetDiskFreeSpaceEx���������Ի�ȡ���������̵Ŀռ�״̬,�������ص��Ǹ�BOOL��������  
			bResult = GetDiskFreeSpaceEx (  
				  dir,  
				  (PULARGE_INTEGER)&i64FreeBytesToCaller,  
				  (PULARGE_INTEGER)&i64TotalBytes,  
				  (PULARGE_INTEGER)&i64FreeBytes);  
			 

			if(bResult)//ͨ�����ص�BOOL�����ж��������Ƿ��ڹ���״̬ ���ڹ���״̬�����Map�� 
			{  
				  m_TotalSize = (int64)(i64TotalBytes/1024/1024);//���������� ����MΪ��λ  
				  m_FreeSize =  (int64)(i64FreeBytesToCaller/1024/1024);//����ʣ��ռ� ����MΪ��λ
				  m_UsedSize = m_TotalSize - m_FreeSize;

				  //����Ӧ����Ϣ����Map��
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