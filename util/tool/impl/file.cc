//#include "stdafx.h"
#include "../file.h"


namespace toolHelp
{

static long tell(int fildes)
{
    return lseek(fildes,0L,SEEK_CUR);
}

CFile::CFile()
{
    strcpy(m_szFileName,"");
    m_bOpenFlag = false;
    m_lMaxTempLength = MAX_BUFF_LEN;
    m_iOpenMode = 0;
}

CFile::~CFile()
{
    if (m_bOpenFlag)
    {
        CloseFile();
    }
}


/**--------------------------------------------------------------
* @param       pFileName : �ļ���
* @param       iMode : �ļ��Ľ�����ʽ
* @return      true : �ɹ�,false : ʧ��
* @description ���������ļ�<br>
               ��ʽ : O_CREAT | O_RDWR | O_TRUNC
----------------------------------------------------------------*/
bool CFile::CreateFile(char *pFileName)
{
    return CreateFile(pFileName,O_CREAT | O_RDWR | O_TRUNC);
}

/**--------------------------------------------------------------
* @param       pFileName : �ļ���
* @param       iMode : �ļ��Ľ�����ʽ
* @return      true : �ɹ�,false : ʧ��
* @description ���������ļ�<br>
               ��ʽ : O_CREAT | O_RDWR | O_BINARY | O_TRUNC
----------------------------------------------------------------*/
bool  CFile::NewFile(char *pFileName)
{
    return CreateFile(pFileName,O_CREAT | O_RDWR | O_TRUNC);
}


/**--------------------------------------------------------------
* @param       pFileName : �ļ���
* @param       iMode : �ļ��Ľ�����ʽ
* @return      true : �ɹ�,false : ʧ��
* @description ���������ļ�
*              open_mode ö�������е�ÿ��ö�ٳ����ĺ������£�
*                 O_RDONLY       ʹ�ļ�ֻ�����������룬�����ж�ȡ���ݡ�
*                 O_WRONLY       ʹ�ļ�ֻ�������������������д�����ݡ�
*                 O_RDWR         ʹ�ļ�ֻ�������ݶ�д����������д���ݡ�
*                 O_NDELAY       UNIX��
*                 O_APPEND       ʹ�ļ�ָ�������ļ�β�������λ�á�
*                 O_TRUNC        ���򿪵��ļ����ڣ��������ȫ�����ݣ�ʹ<br>
*                                ֮��Ϊ���ļ���
*                 O_CREAT        ���򿪵��ļ��������򲻽����������ش�<br>
*                                ʧ����Ϣ��
*                 O_EXCL         ���򿪵��ļ������򷵻ش�ʧ����Ϣ��
*                 O_BINARY       �涨�򿪵�Ϊ�������ļ�������򿪵�Ϊ��<br>
*                                ���ļ���
*                 O_TEXT         �涨�򿪵�Ϊ�ַ��ļ���
*                 O_SYNC         ͬ��д������
*                 O_ASYNC        �첽д������
*  @sample        O_RDWR | O_BINARY �涨�򿪵��ļ��Ƕ���<br>
*                                ���ļ�������ͬʱ��������������
*----------------------------------------------------------------*/
bool CFile::CreateFile(char *pFileName,int iMode)
{
    CloseFile();
    m_FileHandle = open(pFileName,iMode,S_IREAD | S_IWRITE);
    if (m_FileHandle == -1)
    {
        return false;
    }
    m_bOpenFlag = true;
    strcpy(m_szFileName,pFileName);
    m_iOpenMode = iMode;
    return true;
}


/**--------------------------------------------------------------
* @param       pszFileName : �ļ���
* @return      true : �ɹ�,false : ʧ��
* @description ���ļ�<br>
*              ��ʽ : O_RDWR | O_BINARY |  O_EXCL
*---------------------------------------------------------------*/
bool  CFile::OpenFile(char *pszFileName)
{
    return OpenFile(pszFileName, O_RDWR | O_EXCL );
}

/**--------------------------------------------------------------
* @param       pszFileName : �ļ���
* @param       iMode : �ļ��Ĵ򿪷�ʽ
* @return      true : �ɹ�,false : ʧ��
* @description ���ļ�
*---------------------------------------------------------------*/
bool  CFile::OpenFile(char *pszFileName,int iMode)
{
    CloseFile();
    m_FileHandle = open(pszFileName,iMode,S_IRUSR | S_IWUSR |
                    S_IRGRP | S_IWGRP | S_IWOTH | S_IROTH);
    if (m_FileHandle == -1)
    {
        return false;
    }

    m_bOpenFlag = true;
    strcpy(m_szFileName,pszFileName);
    m_iOpenMode = iMode;
    return true;
}

/**--------------------------------------------------------------
* @param       ��
* @return      ��
* @description �ر��ļ�
*---------------------------------------------------------------*/
void  CFile::CloseFile()
{
    if (m_bOpenFlag)
    {
       close(m_FileHandle);
       m_bOpenFlag = false;
    }
}

/**--------------------------------------------------------------
* @param       ��
* @return      ��ǰ�ļ���дָ��λ��
* @description ȡ�ļ���дָ��
*---------------------------------------------------------------*/
long  CFile::GetCurrentPos()
{
    if (!m_bOpenFlag)
       return -1;
    return tell(m_FileHandle);
}

/**--------------------------------------------------------------
* @param       lOffset : �ƶ�ƫ��
* @param       iMode : �ƶ�����<br>
*                 SEEK_SET : �ļ���ͷ <br>
*                 SEEK_CUR : �ļ���ǰλ�� <br>
*                 SEEK_END : �ļ���β <br>
* @return      ��
* @description �ƶ��ļ���дָ��
*---------------------------------------------------------------*/
void  CFile::Lseek(long lOffset,int iMode)
{
    lseek(m_FileHandle,lOffset,iMode);
}

/**--------------------------------------------------------------
* @param       ��
* @return      true : �Ѵ�,false : δ��
* @description �ļ��Ƿ��
*---------------------------------------------------------------*/
bool  CFile::IsFileOpen()
{
    return  m_bOpenFlag;
}



/**--------------------------------------------------------------
* @param       ��
* @return      �ļ�����
* @description ȡ�ļ�����
*---------------------------------------------------------------*/
long  CFile::GetFileLength()
{
    if (!IsFileOpen())
       return -1;
    long iOldPos = tell(m_FileHandle);
    lseek(m_FileHandle,0,SEEK_END);
    long lFileLength = tell(m_FileHandle);
    lseek(m_FileHandle,iOldPos,SEEK_SET);
    return lFileLength;
}

/**--------------------------------------------------------------
* @param       lInsetOffset : ����λ�õ�ƫ��
* @param       pBuffer : ������ָ��
* @param       lBufferLen : ����������
* @return      true : �ɹ�,false : ʧ��
* @description ���ļ�ĳƫ�ƴ�����һ�����ݣ��������ݺ���
*---------------------------------------------------------------*/
bool  CFile::InsetFileBlock(long lInsetOffset,void *pBuffer, long lBufferLen)
{
   if ((lBufferLen <= 0) || (!m_bOpenFlag))
      return false;
   long lFileLen = GetFileLength();
   if (lInsetOffset > lFileLen)
      return false;
   AppendFillBlock(0,lBufferLen);
   MoveFileBlock(lInsetOffset + lBufferLen, lInsetOffset, lFileLen - lInsetOffset);
   Lseek(lInsetOffset,SEEK_SET);
   WriteFile(pBuffer,lBufferLen);
   return  true;
}

/**--------------------------------------------------------------
* @param       lDeleteOffset : ɾ��λ�õ�ƫ��
* @param       lDeleteLen : ɾ������
* @return      true : �ɹ�,false : ʧ��
* @description ���ļ�ĳƫ�ƴ�ɾ��һ��,��������ǰ��
*---------------------------------------------------------------*/
bool  CFile::DeleteFileBlock(long lDeleteOffset,long lDeleteLen)
{
    if (!m_bOpenFlag)
       return false;
    long lFileLen = GetFileLength();
    if ((lFileLen <= lDeleteOffset) || (lDeleteLen <= 0))
       return false;
    long  lOldOff = lDeleteOffset + lDeleteLen;
    if (lOldOff >= lFileLen)
    {
       SetFileLength(lDeleteOffset);
       return true;
    }
    MoveFileBlock(lDeleteOffset,lOldOff,lFileLen - lOldOff);
    SetFileLength(lFileLen - lDeleteLen);
    return  true;
}

/**--------------------------------------------------------------
* @param       pBuffer : ������ָ��
* @param       lBufferLen : ����������
* @return      true : �ɹ�,false : ʧ��
* @description ����һ�����ݵ��ļ�β
*---------------------------------------------------------------*/
bool  CFile::AppendFileBlock(void *pBuffer, long lBufferLen)
{
    if ((!m_bOpenFlag) || (lBufferLen <= 0))
       return  false;
    long lTempLength;
    if (m_lMaxTempLength >= lBufferLen)
       lTempLength = lBufferLen;
    else
       lTempLength = m_lMaxTempLength;
    SeekToEnd();
    char *pTempBuffer = (char *)pBuffer;
    long lCounter = lBufferLen;
    while (lCounter > 0)
    {
       if (lCounter > lTempLength)
       {
          WriteFile(pTempBuffer,lTempLength);
          pTempBuffer += lTempLength;
          lCounter -= lTempLength;
       }
       else
       {
          WriteFile(pTempBuffer, lCounter);
          pTempBuffer += lCounter;
          lCounter = 0;
       }
    }
    return  true;
}

/**--------------------------------------------------------------
* @param       lOffsetDst : ���ƶ���Ŀ��λ�õ�ƫ��
* @param       lOffsetSrc : ���ƶ���Դλ�õ�ƫ��
* @param       lMoveLength : �ƶ��Ŀ鳤��
* @return      true : �ɹ�,false : ʧ��
* @description �ƶ��ļ��飬�򵥸��ǣ��������ԭ���ݺ�
*---------------------------------------------------------------*/
bool CFile::MoveFileBlock(long lOffsetDst,long lOffsetSrc,long lMoveLength)
{
    if (!m_bOpenFlag)
    {
        return false;
    }

    long lFileLen = GetFileLength();
    if (((lOffsetDst + lMoveLength) > lFileLen)
        ||((lOffsetSrc + lMoveLength) > lFileLen))
       return false;
    if (lOffsetDst == lOffsetSrc)
       return true;

    long lTempLength;
    if (m_lMaxTempLength >= lMoveLength)
       lTempLength = lMoveLength;
    else
       lTempLength = m_lMaxTempLength;
    long v_OffsetSrc = lOffsetSrc;
    long v_OffsetDst = lOffsetDst;
    long lCounter = lMoveLength;
    if (lOffsetDst > lOffsetSrc)
    {
       v_OffsetSrc = lOffsetSrc + lMoveLength - lTempLength;
       v_OffsetDst = lOffsetDst + lMoveLength - lTempLength;
    }
    char  *pTempBuffer = new char[lTempLength];
    while (lCounter > 0)
    {
       if (lCounter > lTempLength)
       {
          Lseek(v_OffsetSrc,SEEK_SET);
          ReadFile((void*)pTempBuffer,lTempLength);
          Lseek(v_OffsetDst,SEEK_SET);
          WriteFile((void*)pTempBuffer,lTempLength);
          lCounter -= lTempLength;
       }
       else
       {
          Lseek(v_OffsetSrc,SEEK_SET);
          ReadFile((void*)pTempBuffer, lCounter);
          Lseek(v_OffsetDst,SEEK_SET);
          WriteFile((void*)pTempBuffer, lCounter);
          lCounter = 0;
       }
       if (lOffsetDst < lOffsetSrc)
       {
         v_OffsetSrc += lTempLength;
         v_OffsetDst += lTempLength;
       }
       else
       {
         if (lCounter > lTempLength)
         {
           v_OffsetSrc -= lTempLength;
           v_OffsetDst -= lTempLength;
         }
         else
         {
           v_OffsetSrc -= lCounter;
           v_OffsetDst -= lCounter;
         }
       }
    }
    delete  pTempBuffer;
    return  true;
}

/**--------------------------------------------------------------
* @param       lBufferLen : ׷�ӿտ�ĳ���
* @return      true : �ɹ�,false : ʧ��
* @description ����տ鵽�ļ�β
*---------------------------------------------------------------*/
bool CFile::AppendFileBlock(long lBufferLen)
{
    if ((!m_bOpenFlag) || (lBufferLen <= 0))
    {
        return  false;
    }

    long lTempLength;
    if (m_lMaxTempLength >= lBufferLen)
    {
        lTempLength = lBufferLen;
    }
    else
    {
        lTempLength = m_lMaxTempLength;
    }

    long lCounter = lBufferLen;
    char *pTempBuffer = new char[lTempLength];
    memset(pTempBuffer,0,lTempLength);
    SeekToEnd();
    while (lCounter > 0)
    {
       if (lCounter > lTempLength)
       {
          WriteFile((void*)pTempBuffer, lTempLength);
          lCounter -= lTempLength;
       }
       else
       {
          WriteFile((void*)pTempBuffer, lCounter);
          lCounter = 0;
       }
    }
    delete  pTempBuffer;
    return  true;
}

/**--------------------------------------------------------------
* @param       lOffset : ����λ�õ�ƫ��
* @param       pBuffer : ������ָ��
* @param       lBufferLen : ����������
* @return      true : �ɹ�,false : ʧ��
* @description ���ļ�ĳƫ�ƴ�����һ��
*---------------------------------------------------------------*/
bool CFile::ReplaceFileBlock(long lOffset,void *pBuffer, long lBufferLen)
{
    if ((!m_bOpenFlag)||(!pBuffer))
    {
        return false;
    }

    long lFileLen = GetFileLength();
    if ((lFileLen < (lOffset + lBufferLen)) || (lBufferLen <= 0))
    {
        return false;
    }

    Lseek(lOffset,SEEK_SET);
    WriteFile(pBuffer,lBufferLen);
    return  true;
}

/**--------------------------------------------------------------
* @param       cCode : �����
* @param       lOffset : ���λ�õ�ƫ��
* @param       lLength : ��䳤��
* @return      true : �ɹ�,false : ʧ��
* @description ����ļ���һ��
*---------------------------------------------------------------*/
bool CFile::FileFillBlock(char cCode,long lOffset,long lLength)
{
    if (!m_bOpenFlag)
    {
        return false;
    }

    long lFileLen = GetFileLength();
    if ((lFileLen < (lOffset + lLength)) || (lLength <= 0))
    {
        return false;
    }

    long  lTempLength;
    if (m_lMaxTempLength >= lLength)
    {
        lTempLength = lLength;
    }
    else
    {
        lTempLength = m_lMaxTempLength;
    }

    long lCounter = lLength;
    char *pTempBuffer = new char[lTempLength];
    memset(pTempBuffer,cCode,lTempLength);
    Lseek(lOffset,SEEK_SET);
    while (lCounter > 0)
    {
       if (lCounter > lTempLength)
       {
          WriteFile((void*)pTempBuffer,lTempLength);
          lCounter -= lTempLength;
       }
       else
       {
          WriteFile((void*)pTempBuffer,lCounter);
          lCounter = 0;
       }
    }
    delete  pTempBuffer;
    return  true;
}

/**--------------------------------------------------------------
* @param       cCode : �����
* @param       lLength : ��䳤��
* @return      true : �ɹ�,false : ʧ��
* @description ����տ鵽�ļ�β�����
*---------------------------------------------------------------*/
bool  CFile::AppendFillBlock(char cCode, long lLength)
{
  if ((!m_bOpenFlag) || (lLength <= 0))
  {
      return  false;
  }

  long lTempLength;
  if (m_lMaxTempLength >= lLength)
  {
      lTempLength = lLength;
  }
  else
  {
      lTempLength = m_lMaxTempLength;
  }

  long lCounter = lLength;
  char *pTempBuffer = new char[lTempLength];
  memset(pTempBuffer,cCode,lTempLength);
  SeekToEnd();

  while (lCounter > 0)
  {
     if (lCounter > lTempLength)
     {
        WriteFile((void*)pTempBuffer, lTempLength);
        lCounter -= lTempLength;
     }
     else
     {
        WriteFile((void*)pTempBuffer, lCounter);
        lCounter = 0;
     }
  }
  delete  pTempBuffer;
  return  true;
}

/**--------------------------------------------------------------
* @param       lOffset : �ض��ļ���λ��
* @return      true : �ɹ�,false : ʧ��
* @description �ض��ļ�
*---------------------------------------------------------------*/
bool CFile::TruncatFile(long lOffset)
{
    SetFileLength(lOffset);
    return true;
}

/**--------------------------------------------------------------
* @param       lReadOffset : ����λ�õ�ƫ��
* @param       pBuffer : ���ջ�������ָ��
* @param       lBufferLen : ���ջ������ĳ���
* @return      ʵ�ʶ����ļ��ĳ���,�쳣 = -1
* @description ���ļ�����һ��
*---------------------------------------------------------------*/
long CFile::ReadFile(long lReadOffset,void *pBuffer, long lBufferLen)
{
    if (!m_bOpenFlag)
    {
        return  -1;
    }
    Lseek(lReadOffset,SEEK_SET);
    return read(m_FileHandle,pBuffer,lBufferLen);
}

/**--------------------------------------------------------------
* @param       lWriteOffset : д��λ�õ�ƫ��
* @param       pBuffer : д�뻺������ָ��
* @param       lBufferLen : д�뻺�����ĳ���
* @return      ʵ��д���ļ��ĳ���,�쳣 = -1
* @description дһ�鵽�ļ���
*---------------------------------------------------------------*/
long  CFile::WriteFile(long lWriteOffset,void *pBuffer, long lBufferLen)
{
    if (!m_bOpenFlag)
    {
        return  -1;
    }

    Lseek(lWriteOffset,SEEK_SET);
    return write(m_FileHandle,pBuffer,lBufferLen);
}

/**--------------------------------------------------------------
* @param       pBuffer : ���ջ�������ָ��
* @param       lBufferLen : ���ջ������ĳ���
* @return      ʵ�ʶ����ļ��ĳ���
* @description ���ļ���ǰλ�ö���һ��
*---------------------------------------------------------------*/
long  CFile::ReadFile(void *pBuffer, long lBufferLen)
{
    if (!m_bOpenFlag)
    {
        return  -1;
    }

    return read(m_FileHandle,pBuffer,lBufferLen);
}

/**--------------------------------------------------------------
* @param       pBuffer : д�뻺������ָ��
* @param       lBufferLen : д�뻺�����ĳ���
* @return      ʵ��д���ļ��ĳ���
* @description ���ļ���ǰλ��д��һ��
*---------------------------------------------------------------*/
long  CFile::WriteFile(void *pBuffer, long lBufferLen)
{
    if (!m_bOpenFlag)
    {
        return  -1;
    }

    return write(m_FileHandle,pBuffer,lBufferLen);
}

/**--------------------------------------------------------------
* @param       ��
* @return      ��
* @description �ļ�ָ���ƶ����ļ�β
*---------------------------------------------------------------*/
void  CFile::SeekToEnd()
{
  if (m_bOpenFlag)
  {
      Lseek(0,SEEK_END);
  }
}

/**--------------------------------------------------------------
* @param       lFileLen : �ļ�����
* @return      true : �ɹ�,false : ʧ��
* @description ���ļ�����
*---------------------------------------------------------------*/
bool  CFile::SetFileLength(long lFileLen)
{
    if (!m_bOpenFlag)
    {
        return false;
    }
    
    bool bRet = false;

    int iCurrPos = tell(m_FileHandle);
    if (0 == ftruncate(m_FileHandle,lFileLen))
    {
        bRet = true;
    }
    lseek(m_FileHandle,iCurrPos,SEEK_SET);

    return bRet;
}


/**--------------------------------------------------------------
* @param       ��
* @return      true : ����,false : ������
* @description �ļ��Ƿ����
*---------------------------------------------------------------*/
bool  CFile::IsFileExist(char *pszFileName)
{
    if (access(pszFileName, 0) == 0)
    {
        return  true;
    }
    
    return false;
}


/**--------------------------------------------------------------
* @param       pFileName : �ļ���
* @return      true : �ɹ�,false : ʧ��
* @description ɾ���ļ�
*---------------------------------------------------------------*/
bool  CFile::DeleteFile(char *pFileName)
{
  if(0 == remove(pFileName))
  {
      return true;
  }
  
  return false;
}

/**--------------------------------------------------------------
* @param       pszFileOld : ԭ�ļ���
* @param       pszFileNew : ���ļ���
* @return      true : �ɹ�,false : ʧ��
* @description �ļ�����
*---------------------------------------------------------------*/
bool  CFile::RenameFile(char *pszFileOld,char *pszFileNew)
{
    if(0 == rename(pszFileOld, pszFileNew))
    {
        return true;
    }
    
    return false;
}


}


