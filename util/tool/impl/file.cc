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
* @param       pFileName : 文件名
* @param       iMode : 文件的建立方式
* @return      true : 成功,false : 失败
* @description 建立并打开文件<br>
               方式 : O_CREAT | O_RDWR | O_TRUNC
----------------------------------------------------------------*/
bool CFile::CreateFile(char *pFileName)
{
    return CreateFile(pFileName,O_CREAT | O_RDWR | O_TRUNC);
}

/**--------------------------------------------------------------
* @param       pFileName : 文件名
* @param       iMode : 文件的建立方式
* @return      true : 成功,false : 失败
* @description 建立并打开文件<br>
               方式 : O_CREAT | O_RDWR | O_BINARY | O_TRUNC
----------------------------------------------------------------*/
bool  CFile::NewFile(char *pFileName)
{
    return CreateFile(pFileName,O_CREAT | O_RDWR | O_TRUNC);
}


/**--------------------------------------------------------------
* @param       pFileName : 文件名
* @param       iMode : 文件的建立方式
* @return      true : 成功,false : 失败
* @description 建立并打开文件
*              open_mode 枚举类型中的每个枚举常量的含义如下：
*                 O_RDONLY       使文件只用于数据输入，即从中读取数据。
*                 O_WRONLY       使文件只用于数据输出，即向它写入数据。
*                 O_RDWR         使文件只用于数据读写，即向它读写数据。
*                 O_NDELAY       UNIX用
*                 O_APPEND       使文件指针移至文件尾，即最后位置。
*                 O_TRUNC        若打开的文件存在，则清除其全部内容，使<br>
*                                之变为空文件。
*                 O_CREAT        若打开的文件不存在则不建立它，返回打开<br>
*                                失败信息。
*                 O_EXCL         若打开的文件存在则返回打开失败信息。
*                 O_BINARY       规定打开的为二进制文件，否则打开的为字<br>
*                                符文件。
*                 O_TEXT         规定打开的为字符文件。
*                 O_SYNC         同步写入数据
*                 O_ASYNC        异步写入数据
*  @sample        O_RDWR | O_BINARY 规定打开的文件是二进<br>
*                                制文件，并可同时用于输入和输出。
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
* @param       pszFileName : 文件名
* @return      true : 成功,false : 失败
* @description 打开文件<br>
*              方式 : O_RDWR | O_BINARY |  O_EXCL
*---------------------------------------------------------------*/
bool  CFile::OpenFile(char *pszFileName)
{
    return OpenFile(pszFileName, O_RDWR | O_EXCL );
}

/**--------------------------------------------------------------
* @param       pszFileName : 文件名
* @param       iMode : 文件的打开方式
* @return      true : 成功,false : 失败
* @description 打开文件
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
* @param       无
* @return      无
* @description 关闭文件
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
* @param       无
* @return      当前文件读写指针位置
* @description 取文件读写指针
*---------------------------------------------------------------*/
long  CFile::GetCurrentPos()
{
    if (!m_bOpenFlag)
       return -1;
    return tell(m_FileHandle);
}

/**--------------------------------------------------------------
* @param       lOffset : 移动偏移
* @param       iMode : 移动方向<br>
*                 SEEK_SET : 文件开头 <br>
*                 SEEK_CUR : 文件当前位置 <br>
*                 SEEK_END : 文件结尾 <br>
* @return      无
* @description 移动文件读写指针
*---------------------------------------------------------------*/
void  CFile::Lseek(long lOffset,int iMode)
{
    lseek(m_FileHandle,lOffset,iMode);
}

/**--------------------------------------------------------------
* @param       无
* @return      true : 已打开,false : 未打开
* @description 文件是否打开
*---------------------------------------------------------------*/
bool  CFile::IsFileOpen()
{
    return  m_bOpenFlag;
}



/**--------------------------------------------------------------
* @param       无
* @return      文件长度
* @description 取文件长度
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
* @param       lInsetOffset : 插入位置的偏移
* @param       pBuffer : 缓冲区指针
* @param       lBufferLen : 缓冲区长度
* @return      true : 成功,false : 失败
* @description 在文件某偏移处插入一块内容，后面内容后移
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
* @param       lDeleteOffset : 删除位置的偏移
* @param       lDeleteLen : 删除长度
* @return      true : 成功,false : 失败
* @description 在文件某偏移处删除一块,后面内容前移
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
* @param       pBuffer : 缓冲区指针
* @param       lBufferLen : 缓冲区长度
* @return      true : 成功,false : 失败
* @description 加入一块内容到文件尾
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
* @param       lOffsetDst : 块移动的目标位置的偏移
* @param       lOffsetSrc : 块移动的源位置的偏移
* @param       lMoveLength : 移动的块长度
* @return      true : 成功,false : 失败
* @description 移动文件块，简单覆盖，并不会把原内容后挤
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
* @param       lBufferLen : 追加空块的长度
* @return      true : 成功,false : 失败
* @description 加入空块到文件尾
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
* @param       lOffset : 覆盖位置的偏移
* @param       pBuffer : 缓冲区指针
* @param       lBufferLen : 缓冲区长度
* @return      true : 成功,false : 失败
* @description 在文件某偏移处覆盖一块
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
* @param       cCode : 填充码
* @param       lOffset : 填充位置的偏移
* @param       lLength : 填充长度
* @return      true : 成功,false : 失败
* @description 填充文件的一块
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
* @param       cCode : 填充码
* @param       lLength : 填充长度
* @return      true : 成功,false : 失败
* @description 加入空块到文件尾并填充
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
* @param       lOffset : 截断文件的位置
* @return      true : 成功,false : 失败
* @description 截断文件
*---------------------------------------------------------------*/
bool CFile::TruncatFile(long lOffset)
{
    SetFileLength(lOffset);
    return true;
}

/**--------------------------------------------------------------
* @param       lReadOffset : 读出位置的偏移
* @param       pBuffer : 接收缓冲区的指针
* @param       lBufferLen : 接收缓冲区的长度
* @return      实际读出文件的长度,异常 = -1
* @description 从文件读出一块
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
* @param       lWriteOffset : 写入位置的偏移
* @param       pBuffer : 写入缓冲区的指针
* @param       lBufferLen : 写入缓冲区的长度
* @return      实际写入文件的长度,异常 = -1
* @description 写一块到文件内
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
* @param       pBuffer : 接收缓冲区的指针
* @param       lBufferLen : 接收缓冲区的长度
* @return      实际读出文件的长度
* @description 从文件当前位置读出一块
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
* @param       pBuffer : 写入缓冲区的指针
* @param       lBufferLen : 写入缓冲区的长度
* @return      实际写入文件的长度
* @description 从文件当前位置写入一块
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
* @param       无
* @return      无
* @description 文件指针移动到文件尾
*---------------------------------------------------------------*/
void  CFile::SeekToEnd()
{
  if (m_bOpenFlag)
  {
      Lseek(0,SEEK_END);
  }
}

/**--------------------------------------------------------------
* @param       lFileLen : 文件长度
* @return      true : 成功,false : 失败
* @description 设文件长度
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
* @param       无
* @return      true : 存在,false : 不存在
* @description 文件是否存在
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
* @param       pFileName : 文件名
* @return      true : 成功,false : 失败
* @description 删除文件
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
* @param       pszFileOld : 原文件名
* @param       pszFileNew : 新文件名
* @return      true : 成功,false : 失败
* @description 文件改名
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


