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
    //��ȡ���������������ݳ���
    int32   GetDataLength();
    //��ȡ�ֽڻ������Ĵ�С
    int32   GetBufferSize();
    //������ݣ��ͷŻ������ռ�
    void    ClearBuffer();
    //������ݣ������ͷŻ������ռ�
    void    ClearData();
    //��ȡ������ָ��
    void*   GetBuffer();

    //�ӻ�����ͷ��ɾ�����ݣ��������ռ䲻��
    BOOL    Delete(int32 nLength);
    //�ӻ�����β��ɾ�����ݣ��������ռ䲻��
    BOOL    DeleteTail(int32 nLength);
    //�ӻ���ͷ����������,�����������Ӧ����
    int32   Read(void *pData, int32 nSize);
    //��ȡ��һ�����ݣ���δ��ȡ�����򷵻�NULL
    char*   ReadLine();
    //�ӻ���β��д������
    BOOL    Write(void* pData, int32 nSize);
    //�ӻ���β��д������
    BOOL    Write(CBuffer *pBuffer);
    //�ӻ���ͷ����������
    BOOL    Insert(void* pData,int32 nSize);
    //�ڻ������в���һ���ִ�
    int32   Find(char* key, int32 nPos);

    //ѹ���������ռ䵽���ݳ��ȴ�С
    BOOL    Compress();
    //��һ���������������ݵ���һ��������
    BOOL    Copy(CBuffer& buffer);

public:
    //������������д���ļ�(��������)
    BOOL    WriteToFile(const char* strFileName);

private:
    BOOL    ReAllocateBuffer(int32 nRequestedSize);

    CBuffer(CBuffer &);
    CBuffer& operator=(CBuffer &);
    pthread_mutex_t  m_mutex;     //Buffer��

private:
    void*   m_pBase;               //�ֽڻ�����ָ��
    int32   m_nDataSize;           //��������С
    int32   m_nBufferSize;         //��������С
};


}

#endif /* BUFFER_H_ */
