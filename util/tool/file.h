#ifndef FILE_H_
#define FILE_H_

#include "../base/base.h"
#include "../base/data_types.h"


namespace toolHelp
{

#define MAX_BUFF_LEN   4096

class CFile
{
public:
        CFile();
        virtual ~CFile();
        
        //建立并打开文件
        bool    CreateFile(char *pFileName);
        //建立并打开长度为0的新文件(旧文件若存在，则截断)
        bool    NewFile(char *pFileName);
        //建立并打开文件
        bool    CreateFile(char *pFileName,int iMode);
        //打开文件
        bool    OpenFile(char *pszFileName);
        //以特定模式打开文件
        bool    OpenFile(char *pszFileName,int iMode);
        //关闭打开的文件
        void    CloseFile();
        //获取当前文件读写指针位置
        long    GetCurrentPos();
        //移动文件指针位置
        void    Lseek(long lOffset,int iMode);
        //判断文件是否已经打开
        bool    IsFileOpen();
        //获取文件长度
        long    GetFileLength();
        //在文件某偏移处插入一块内容，后面内容后移
        bool    InsetFileBlock(long lInsetOffset,void *pBuffer, long lBufferLen);
        //在文件某偏移处删除一块,后面内容前移
        bool    DeleteFileBlock(long lDeleteOffset,long lDeleteLen);
        //加入一块内容到文件尾
        bool    AppendFileBlock(void *pBuffer, long lBufferLen);
        //移动文件块，简单覆盖，并不会把原内容后挤
        bool    MoveFileBlock(long lOffsetDst,long lOffsetSrc,long lMoveLength);
        //加入空块到文件尾(内容为0)
        bool    AppendFileBlock(long lBufferLen);
        //在文件某偏移处替换一块
        bool    ReplaceFileBlock(long lOffset,void *pBuffer, long lBufferLen);
        //填充文件的一块
        bool    FileFillBlock(char cCode,long lOffset,long lLength);
        //加入空块到文件尾并填充
        bool    AppendFillBlock(char cCode, long lLength);
        //截断文件长度
        bool    TruncatFile(long lOffset);
        //从文件读出一块内容到缓冲区
        long    ReadFile(long lReadOffset,void *pBuffer, long lBufferLen);
        //写入缓冲区内容到文件
        long    WriteFile(long lWriteOffset,void *pBuffer, long lBufferLen);
        //从文件当前位置读出一块内空到缓冲区
        long    ReadFile(void *pBuffer, long lBufferLen);
        //写入缓冲区内容到文件当前位置
        long    WriteFile(void *pBuffer, long lBufferLen);
        //移动文件指针到文件尾
        void    SeekToEnd();
        //设文件长度
        bool    SetFileLength(long lFileLen);

public:
        //判断文件是否存在
        static bool  IsFileExist(char *pszFileName);
        //删除特写文件
        static bool  DeleteFile(char *pFileName);
        //文件改名
        static bool  RenameFile(char *pszFileOld,char *pszFileNew);
        //拷贝文件数据
        static bool  CopyFile(char *pszFileDst,char *pszFileSrc);


private:
        char   m_szFileName[255];       //文件名称
        int    m_lMaxTempLength;        //最大缓冲区长度
        int    m_FileHandle;            //文件句柄
        bool   m_bOpenFlag;             //文件打开标志
        int    m_iOpenMode;             //文件打开模式
};


}


#endif /* FILE_H_ */
