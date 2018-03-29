//#include "stdafx.h"
#include "../buffer.h"


namespace toolHelp
{

#define BUFFER_STEP_LENGTH 512    //缓冲区增加步长，缺省为512

CBuffer::CBuffer()
{
    m_nBufferSize = 0;
    m_nDataSize = 0;
    m_pBase = NULL;
    //初始化互斥锁
    pthread_mutex_init(&m_mutex,NULL);
}

CBuffer::~CBuffer()
{
    pthread_mutex_destroy(&m_mutex);
    if (NULL != m_pBase && m_nBufferSize > 0)
    {
        free(m_pBase);
        m_pBase = NULL;
    }
}

////////////////////////////////////////////////////////////////////////////////
//
// 函数名称:    GetBufferSize
//
// 函数说明:    获取字节缓冲区的大小
//
// 参数说明:
//
// 返 回 值:    字节缓冲区的大小
//
// 重要说明:
//
////////////////////////////////////////////////////////////////////////////////
int32  CBuffer::GetBufferSize()
{
    return m_nBufferSize;
}

////////////////////////////////////////////////////////////////////////////////
//
// 函数名称:    GetDataLength
//
// 函数说明:    获取缓冲区中填充的数据长度
//
// 参数说明:
//
// 返 回 值:    数据长度
//
// 重要说明:
//
////////////////////////////////////////////////////////////////////////////////
int32  CBuffer::GetDataLength()
{
    if (m_pBase == NULL)
    {
        return 0;
    }

    return m_nDataSize;
}


////////////////////////////////////////////////////////////////////////////////
//
// 函数名称:    ClearBuffer
//
// 函数说明:    清除数据，释放缓冲区空间
//
// 参数说明:
//
// 返 回 值:    无
//
// 重要说明:
//
////////////////////////////////////////////////////////////////////////////////
void  CBuffer::ClearBuffer()
{
    pthread_mutex_lock(&m_mutex);
    if(NULL != m_pBase && m_nBufferSize > 0)
    {
        free(m_pBase);
        m_pBase = NULL;
    }

    m_nBufferSize = 0;
    m_nDataSize = 0;
    pthread_mutex_unlock(&m_mutex);
}

////////////////////////////////////////////////////////////////////////////////
//
// 函数名称:    ClearData
//
// 函数说明:    清除数据，但不释放缓冲区空间
//
// 参数说明:
//
// 返 回 值:    无
//
// 重要说明:
//
////////////////////////////////////////////////////////////////////////////////
void  CBuffer::ClearData()
{
    pthread_mutex_lock(&m_mutex);
    if(NULL != m_pBase)
    {
        memset(m_pBase,0,m_nDataSize);
        m_nDataSize = 0;
    }
    pthread_mutex_unlock(&m_mutex);
}


////////////////////////////////////////////////////////////////////////////////
//
// 函数名称:    GetBuffer
//
// 函数说明:    获取缓冲区指针
//
// 参数说明:
//
// 返 回 值:    获取的缓冲区指针
//
// 重要说明:
//
////////////////////////////////////////////////////////////////////////////////
void*  CBuffer::GetBuffer()
{
   return m_pBase;
}


////////////////////////////////////////////////////////////////////////////////
//
// 函数名称:    Delete
//
// 函数说明:    从缓冲区头部删除数据，缓冲区空间不变
//
// 参数说明:    nLength       需要删除的长度，不能大于数据长度，若大于数据
//                            长度，则删除失败
//
// 返 回 值:    TRUE: 成功    FALSE: 失败
//
// 重要说明:
//
////////////////////////////////////////////////////////////////////////////////
BOOL  CBuffer::Delete(int32 nLength)
{
    pthread_mutex_lock(&m_mutex);
    if(nLength > m_nDataSize)
    {
        pthread_mutex_unlock(&m_mutex);
        return FALSE;
    }
    if(nLength > 0)
    {
        memmove(m_pBase,(char*)m_pBase + nLength,m_nBufferSize - nLength);
        m_nDataSize = m_nDataSize - nLength;
        memset((char*)m_pBase + m_nDataSize,0,nLength);
        pthread_mutex_unlock(&m_mutex);
        return TRUE;
    }
    pthread_mutex_unlock(&m_mutex);
    return FALSE;
}

////////////////////////////////////////////////////////////////////////////////
//
// 函数名称:    DeleteTail
//
// 函数说明:    从缓冲区尾部删除数据，缓冲区空间不变
//
// 参数说明:    nLength       需要删除的长度，不能大于数据长度，若大于数据
//                            长度，则删除失败
//
// 返 回 值:    TRUE: 成功    FALSE: 失败
//
// 重要说明:
//
////////////////////////////////////////////////////////////////////////////////
BOOL  CBuffer::DeleteTail(int32 nLength)
{
    pthread_mutex_lock(&m_mutex);
    if(nLength > m_nDataSize)
    {
        pthread_mutex_unlock(&m_mutex);
        return FALSE;
    }

    if(nLength > 0)
    {
        m_nDataSize = m_nDataSize - nLength;
        memset((char*)m_pBase + m_nDataSize,0,nLength);
        pthread_mutex_unlock(&m_mutex);
        return TRUE;
    }
    pthread_mutex_unlock(&m_mutex);
    return FALSE;
}

////////////////////////////////////////////////////////////////////////////////
//
// 函数名称:    Read
//
// 函数说明:    从缓冲头部读出数据,缓冲区清除相应数据
//
// 参数说明:    pData          存放读取数据的缓冲区指针
//              nSize          缓冲区长度
//
// 返 回 值:    实际读取到的数据长度，不会大于其实际保存的数据长度
//
// 重要说明:
//
////////////////////////////////////////////////////////////////////////////////
int32  CBuffer::Read(void *pData, int32 nSize)
{
    pthread_mutex_lock(&m_mutex);
    if(nSize > m_nDataSize)
    {
        nSize = m_nDataSize;
    }

    if(nSize > 0)
    {
        memcpy((char*)pData,(char*)m_pBase,nSize);
        memmove(m_pBase,(char*)m_pBase + nSize,m_nBufferSize - nSize);
        m_nDataSize = m_nDataSize - nSize;
        memset((char*)m_pBase + m_nDataSize,0,nSize);
    }
    pthread_mutex_unlock(&m_mutex);
    return nSize;
}

////////////////////////////////////////////////////////////////////////////////
//
// 函数名称:    ReadLine
//
// 函数说明:    从缓冲头部开始读出一行数据
//
// 参数说明:
//
// 返 回 值:    读取的一行数据，若未读取到，则返回NULL
//
// 重要说明:    返回的数据是新开辟的空间，需要自己清除，返回数据中包含最后的
//              行结束符
//
////////////////////////////////////////////////////////////////////////////////
char*  CBuffer::ReadLine()
{
    char *lineBuf = NULL;
    int i = 0;

    for(i = 0; i< m_nDataSize; i++ )
    {
        char *pTemp = (char*)m_pBase + i;
        if((*pTemp) == '\n')
        {
            break;
        }
    }

    if(i == m_nDataSize)
    {
         return NULL;
    }

    if((lineBuf = (char*)malloc(i+2)) == NULL)  //分配空间失败
    {
        return NULL;
    }

    memcpy(lineBuf,m_pBase,i+1);
    lineBuf[i+1] = '\0';

    Delete(i+1);

    return lineBuf;
}

////////////////////////////////////////////////////////////////////////////////
//
// 函数名称:    Insert
//
// 函数说明:    从缓冲头部插入数据
//
// 参数说明:    pData          存放需插入数据的指针
//              nSize          需插入数据长度
//
// 返 回 值:    TRUE   插入成功     FALSE   插入失败
//
// 重要说明:
//
////////////////////////////////////////////////////////////////////////////////
BOOL  CBuffer::Insert(void* pData,int32 nSize)
{
    pthread_mutex_lock(&m_mutex);
    if(ReAllocateBuffer(nSize + m_nDataSize))
    {
       memmove((char*)m_pBase + nSize,(char*)m_pBase,m_nBufferSize - nSize);
       memcpy((char*)m_pBase, (char*)pData, nSize);
       m_nDataSize += nSize;
	   pthread_mutex_unlock(&m_mutex);
       return TRUE;
    }
    pthread_mutex_unlock(&m_mutex);
    return FALSE;
}


////////////////////////////////////////////////////////////////////////////////
//
// 函数名称:    Write
//
// 函数说明:    从缓冲尾部写入数据
//
// 参数说明:    pData          存放需写入数据的指针
//              nSize          需写入数据长度
//
// 返 回 值:    TRUE   写入成功     FALSE   写入失败
//
// 重要说明:
//
////////////////////////////////////////////////////////////////////////////////
BOOL  CBuffer::Write(void* pData, int32 nSize)
{
    pthread_mutex_lock(&m_mutex);
    if (ReAllocateBuffer(m_nDataSize + nSize))
    {
        memcpy((char*)m_pBase + m_nDataSize, (char*)pData, nSize);
        m_nDataSize += nSize;
        pthread_mutex_unlock(&m_mutex);
        return TRUE;
    }
    pthread_mutex_unlock(&m_mutex);
    return FALSE;
}

////////////////////////////////////////////////////////////////////////////////
//
// 函数名称:    Write
//
// 函数说明:    从缓冲尾部写入数据
//
// 参数说明:    pBuffer        保存数据的缓冲对象
//
// 返 回 值:    TRUE   写入成功     FALSE   写入失败
//
// 重要说明:
//
////////////////////////////////////////////////////////////////////////////////
BOOL  CBuffer::Write(CBuffer *pBuffer)
{
     return Write(pBuffer->GetBuffer(), pBuffer->GetDataLength());
}



////////////////////////////////////////////////////////////////////////////////
//
// 函数名称:    Compress
//
// 函数说明:    压缩缓冲区空间到数据长度大小
//
// 参数说明:
//
// 返 回 值:    TRUE   压缩成功     FALSE   压缩失败
//
// 重要说明:
//
////////////////////////////////////////////////////////////////////////////////
BOOL  CBuffer::Compress()
{
    pthread_mutex_lock(&m_mutex);
    if(NULL != m_pBase)
    {
        m_nBufferSize = m_nDataSize + 1;
        m_pBase = realloc(m_pBase,m_nBufferSize);
        memset((char*)m_pBase + m_nDataSize,0,m_nBufferSize - m_nDataSize);
        pthread_mutex_unlock(&m_mutex);
        return TRUE;
    }
    pthread_mutex_unlock(&m_mutex);
    return FALSE;
}


////////////////////////////////////////////////////////////////////////////////
//
// 函数名称:    Copy
//
// 函数说明:    从一个缓冲区拷贝数据到另一个缓冲区
//
// 参数说明:
//
// 返 回 值:    TRUE   拷贝成功     FALSE   拷贝失败
//
// 重要说明:
//
//
////////////////////////////////////////////////////////////////////////////////
BOOL  CBuffer::Copy(CBuffer& buffer)
{
	pthread_mutex_lock(&m_mutex);
    int nSrcBufferSize = buffer.GetBufferSize();
    if(nSrcBufferSize)
    {
        ClearBuffer();
        m_pBase = calloc(nSrcBufferSize,sizeof(char));    //空间自动初始化为0
        if(NULL != m_pBase)
        {
            memcpy(m_pBase,buffer.GetBuffer(),buffer.GetDataLength());
            m_nBufferSize = nSrcBufferSize;
            m_nDataSize = buffer.GetDataLength();
            memset((char*)m_pBase + m_nDataSize, 0, m_nBufferSize - m_nDataSize);
            pthread_mutex_unlock(&m_mutex);
            return TRUE;
        }
    }
    pthread_mutex_unlock(&m_mutex);
    return FALSE;
}

////////////////////////////////////////////////////////////////////////////////
//
// 函数名称:    Find
//
// 函数说明:    在缓冲区中查找一个字串
//
// 参数说明:    key          需要查找的字串
//              nPos         查找开始位置
//
// 返 回 值:    若未查找到，则返回-1，否则返回字串的位置偏移，此位置偏移已经包含
//              了字串的长度
//
// 重要说明:
//
//
////////////////////////////////////////////////////////////////////////////////
int CBuffer::Find(char *key, int32 nPos)
{
    if (nPos > m_nDataSize)
    {
        return -1;
    }

    char *pStr = (char*) strstr((char*)m_pBase + nPos, key);

    int nOffset = 0;
    if (pStr)
    {
        nOffset = (pStr - (char*)m_pBase) + strlen(key);
    }

    return nOffset;
}

BOOL  CBuffer::ReAllocateBuffer(int32 nRequestedSize)
{
    if (nRequestedSize < m_nBufferSize) //缓冲区够大，还不需要增加缓冲区
    {
        return TRUE;
    }

    int32 nNewSize = (int32) ceil(nRequestedSize / (BUFFER_STEP_LENGTH * 1.0)) * BUFFER_STEP_LENGTH;

    //分配新的缓冲区
    char *pTemp = (char*)calloc(nNewSize,sizeof(char));
    if(pTemp != NULL)
    {
        memcpy(pTemp, (char*)m_pBase, m_nDataSize);  //把旧数据拷贝到新缓冲区

        free(m_pBase);  //清除以前的数据
        m_nBufferSize = nNewSize;
        m_pBase = pTemp;

        return TRUE;
    }

    return FALSE;
}

////////////////////////////////////////////////////////////////////////////////
//
// 函数名称:    WriteToFile
//
// 函数说明:    把缓冲区数据写入件
//
// 参数说明:
//
// 返 回 值:    TRUE   写入成功     FALSE   写入失败
//
// 重要说明:
//
//
////////////////////////////////////////////////////////////////////////////////
BOOL CBuffer::WriteToFile(const char* strFileName)
{
    FILE*  pFile = NULL;

    pFile = fopen(strFileName,"wb");
    if(pFile != NULL)
    {
        fwrite(m_pBase,1,m_nBufferSize,pFile);
        fflush(pFile);
        fclose(pFile);

        return TRUE;
    }

    return FALSE;
}

}
