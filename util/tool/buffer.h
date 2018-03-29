#ifndef BUFFER_H_
#define BUFFER_H_


#include "../base/base.h"
#include "../base/data_types.h"


namespace toolHelp
{

class CBuffer
{
public:
    CBuffer();
    virtual ~CBuffer();
    //获取缓冲区中填充的数据长度
    int32   GetDataLength();
    //获取字节缓冲区的大小
    int32   GetBufferSize();
    //清除数据，释放缓冲区空间
    void    ClearBuffer();
    //清除数据，但不释放缓冲区空间
    void    ClearData();
    //获取缓冲区指针
    void*   GetBuffer();

    //从缓冲区头部删除数据，缓冲区空间不变
    BOOL    Delete(int32 nLength);
    //从缓冲区尾部删除数据，缓冲区空间不变
    BOOL    DeleteTail(int32 nLength);
    //从缓冲头部读出数据,缓冲区清除相应数据
    int32   Read(void *pData, int32 nSize);
    //读取的一行数据，若未读取到，则返回NULL
    char*   ReadLine();
    //从缓冲尾部写入数据
    BOOL    Write(void* pData, int32 nSize);
    //从缓冲尾部写入数据
    BOOL    Write(CBuffer *pBuffer);
    //从缓冲头部插入数据
    BOOL    Insert(void* pData,int32 nSize);
    //在缓冲区中查找一个字串
    int32   Find(char* key, int32 nPos);

    //压缩缓冲区空间到数据长度大小
    BOOL    Compress();
    //从一个缓冲区拷贝数据到另一个缓冲区
    BOOL    Copy(CBuffer& buffer);

public:
    //把整个缓冲区写入文件(包含数据)
    BOOL    WriteToFile(const char* strFileName);

private:
    BOOL    ReAllocateBuffer(int32 nRequestedSize);

    CBuffer(CBuffer &);
    CBuffer& operator=(CBuffer &);
    pthread_mutex_t  m_mutex;     //Buffer锁

private:
    void*   m_pBase;               //字节缓冲区指针
    int32   m_nDataSize;           //数据区大小
    int32   m_nBufferSize;         //缓冲区大小
};


}

#endif /* BUFFER_H_ */
