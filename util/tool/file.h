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
        
        //���������ļ�
        bool    CreateFile(char *pFileName);
        //�������򿪳���Ϊ0�����ļ�(���ļ������ڣ���ض�)
        bool    NewFile(char *pFileName);
        //���������ļ�
        bool    CreateFile(char *pFileName,int iMode);
        //���ļ�
        bool    OpenFile(char *pszFileName);
        //���ض�ģʽ���ļ�
        bool    OpenFile(char *pszFileName,int iMode);
        //�رմ򿪵��ļ�
        void    CloseFile();
        //��ȡ��ǰ�ļ���дָ��λ��
        long    GetCurrentPos();
        //�ƶ��ļ�ָ��λ��
        void    Lseek(long lOffset,int iMode);
        //�ж��ļ��Ƿ��Ѿ���
        bool    IsFileOpen();
        //��ȡ�ļ�����
        long    GetFileLength();
        //���ļ�ĳƫ�ƴ�����һ�����ݣ��������ݺ���
        bool    InsetFileBlock(long lInsetOffset,void *pBuffer, long lBufferLen);
        //���ļ�ĳƫ�ƴ�ɾ��һ��,��������ǰ��
        bool    DeleteFileBlock(long lDeleteOffset,long lDeleteLen);
        //����һ�����ݵ��ļ�β
        bool    AppendFileBlock(void *pBuffer, long lBufferLen);
        //�ƶ��ļ��飬�򵥸��ǣ��������ԭ���ݺ�
        bool    MoveFileBlock(long lOffsetDst,long lOffsetSrc,long lMoveLength);
        //����տ鵽�ļ�β(����Ϊ0)
        bool    AppendFileBlock(long lBufferLen);
        //���ļ�ĳƫ�ƴ��滻һ��
        bool    ReplaceFileBlock(long lOffset,void *pBuffer, long lBufferLen);
        //����ļ���һ��
        bool    FileFillBlock(char cCode,long lOffset,long lLength);
        //����տ鵽�ļ�β�����
        bool    AppendFillBlock(char cCode, long lLength);
        //�ض��ļ�����
        bool    TruncatFile(long lOffset);
        //���ļ�����һ�����ݵ�������
        long    ReadFile(long lReadOffset,void *pBuffer, long lBufferLen);
        //д�뻺�������ݵ��ļ�
        long    WriteFile(long lWriteOffset,void *pBuffer, long lBufferLen);
        //���ļ���ǰλ�ö���һ���ڿյ�������
        long    ReadFile(void *pBuffer, long lBufferLen);
        //д�뻺�������ݵ��ļ���ǰλ��
        long    WriteFile(void *pBuffer, long lBufferLen);
        //�ƶ��ļ�ָ�뵽�ļ�β
        void    SeekToEnd();
        //���ļ�����
        bool    SetFileLength(long lFileLen);

public:
        //�ж��ļ��Ƿ����
        static bool  IsFileExist(char *pszFileName);
        //ɾ����д�ļ�
        static bool  DeleteFile(char *pFileName);
        //�ļ�����
        static bool  RenameFile(char *pszFileOld,char *pszFileNew);
        //�����ļ�����
        static bool  CopyFile(char *pszFileDst,char *pszFileSrc);


private:
        char   m_szFileName[255];       //�ļ�����
        int    m_lMaxTempLength;        //��󻺳�������
        int    m_FileHandle;            //�ļ����
        bool   m_bOpenFlag;             //�ļ��򿪱�־
        int    m_iOpenMode;             //�ļ���ģʽ
};


}


#endif /* FILE_H_ */
