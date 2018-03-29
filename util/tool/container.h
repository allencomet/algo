#ifndef CONTAINER_H_
#define CONTAINER_H_

#include "../base/data_types.h"
#include "syncobject.h"

namespace toolHelp
{

class CPtrArrayList
{
public:
    static const int32 LAST_INDEX;

    CPtrArrayList(int32 initCount = 2);
    virtual ~CPtrArrayList();

    int32 GetCount() const;
    bool  Append(void* value);
    const void* GetItem(int32 index) const;
    void* TakeItem(int32 index);
    void  Clear();

private:
    void  Init();
    CPtrArrayList(CPtrArrayList &);
    CPtrArrayList& operator=(CPtrArrayList &);

    int32 mInitCount;
    int32 mMaxCount;
    int32 mCount;
    void ** mFirst;
};

class CPtrCircleQueue
{
public:
    CPtrCircleQueue();
    virtual  ~CPtrCircleQueue();

    void  Push(void *item);
	void*  Pop();
	void*  Front();
    void*  Top();
    int  GetLength();

private:
    void ** mEntries;
    unsigned int mHead;
    unsigned int mTail;
    unsigned int mCount;
    unsigned int mMaxCount;
};


class CBlockingQueue
{
public:
    CBlockingQueue();
    virtual ~CBlockingQueue();

    //��������������ݽ�����
    void Push(void *item);

    //��������û�����ݣ���ȴ��������ݲŷ���
    void* Pop();

	//��������û�����ݣ���ȴ��������ݲŷ���,���Ҳ���ɾ��������Ԫ��
	void* Front();

    //����������û�����ݣ���ֱ�ӷ���NULL
    void* Top();

    //�����������ض��г���
    int GetLength();

private:
    CPtrCircleQueue * mQueue;
    pthread_mutex_t mMutex;
    pthread_cond_t mCond;
};



/*********************************
�����б����
**********************************/
template<typename V>  class CArrayList
{
public:

    CArrayList() {}
    ~CArrayList()
    {
        if(!valueVector.empty())
        {
            valueVector.clear();
        }
    }

    //��ָ����Ԫ����ӵ����б��β��
    void  Add(V &value)
    {
        valueVector.push_back(value);
    }

    //��ָ����Ԫ�ز�����б��е�ָ��λ��(ע��:index����0<=index<Size)
    void Add(int index, V &value)
    {
        if(index >=0 && index < valueVector.size())
        {
            valueVector.insert(valueVector.begin() + index,value);
        }
    }

    //�Ƴ����б��е�����Ԫ��
    void Clear()
    {
        valueVector.clear();
    }

    //�ж��б����Ƿ����ĳԪ��
    bool  Contains(V &value)
    {
        typename std::vector<V>::iterator iter;
        bool bRet = false;

        for (iter = valueVector.begin(); iter != valueVector.end(); ++iter)
        {
            if((*iter) == value)
            {
                bRet = true;
                break;
            }
        }

        return bRet;
    }

    //���ش��б���ָ��λ���ϵ�Ԫ��(ע��:index����0<=index<Size);
    V  Get(int index)
    {
        V tempValue = V();

        if(index >=0 && index < valueVector.size())
        {
            tempValue = valueVector[index];
        }

        return tempValue;
    }

    //���ش��б����״γ��ֵ�ָ��Ԫ�ص���������������б�����Ԫ�أ��򷵻�-1
    int IndexOf(V &value)
    {
        typename std::vector<V>::iterator iter;
        int nRet = -1;

        for (iter = valueVector.begin(); iter != valueVector.end(); ++iter)
        {
            if((*iter) == value)
            {
                nRet = iter - valueVector.begin();
                break;
            }
        }

        return nRet;
    }

    //�ж��б��Ƿ�Ϊ��
    bool IsEmpty()
    {
        bool bRet = false;
        bRet = valueVector.empty();
        return bRet;
    }


    //���ش��б������һ�γ��ֵ�ָ��Ԫ�ص���������������б������������򷵻�-1��
    int LastIndexOf(V &value)
    {
        typename std::vector<V>::iterator iter;
        int nRet = -1;

        for (iter = valueVector.rbegin(); iter != valueVector.rend(); ++iter)
        {
            if((*iter) == value)
            {
                nRet = iter - valueVector.rend();
                break;
            }
        }

        return nRet;
    }

    //�Ƴ����б���ָ��λ���ϵ�Ԫ��(ע��:index����0<=index<Size)
    void Remove(int index)
    {
        if(index >=0 && index < valueVector.size())
        {
            valueVector.erase(valueVector.begin() + index);
        }
    }

    //�Ƴ����б����״γ��ֵ�ָ��Ԫ��(�������)
    bool  Remove(V &value)
    {
        typename std::vector<V>::iterator iter;
        bool bRet = false;

        for (iter = valueVector.begin(); iter != valueVector.end();)
        {
            if((*iter) == value)
            {
                valueVector.erase(iter);
                bRet = true;
                break;
            }
        }

        return bRet;
    }

    //�Ƴ����б��г��ֵ�ָ��Ԫ��(������ڶ������ȫ���Ƴ�)
    bool  RemoveAll(V &value)
    {
        typename std::vector<V>::iterator iter;
        bool bRet = false;

        for (iter = valueVector.begin(); iter != valueVector.end();)
        {
            if((*iter) == value)
            {
                iter = valueVector.erase(iter);
                bRet = true;
                break;
            }
            else
            {
                ++iter;
            }
        }

        return bRet;
    }

    //��ָ����Ԫ��������б���ָ��λ���ϵ�Ԫ��(ע��:index����0<=index<Size)
    void Set(int index, V &value)
    {
        if(index >=0 && index < valueVector.size())
        {
            valueVector[index] = value;
        }
    }

    //���ش��б��е�Ԫ����
    uint32 Size()
    {
        return valueVector.size();
    }

    //��˳�򷵻ذ������б�������Ԫ�ص�vector
    void ToArray(std::vector<V> &valueVec)
    {
        copy(valueVector.begin(),valueVector.end(),back_inserter(valueVec));
    }

private:
    std::vector<V> valueVector;

private:
    DISALLOW_COPY_AND_ASSIGN(CArrayList);
};

/*********************************
�̲߳�������
**********************************/
template<typename V>  class CConcurrentArrayList
{
public:

    CConcurrentArrayList() {}
    ~CConcurrentArrayList()
    {
        if(!valueVector.empty())
        {
            valueVector.clear();
        }
    }

    //��ָ����Ԫ����ӵ����б��β��
    void  Add(V &value)
    {
        mutexObject.Lock();
        valueVector.push_back(value);
        mutexObject.Unlock();
    }

    //��ָ����Ԫ�ز�����б��е�ָ��λ��(ע��:index����0<=index<Size)
    void Add(int index, V &value)
    {
        mutexObject.Lock();
        if(index >=0 && index < valueVector.size())
        {
            valueVector.insert(valueVector.begin() + index,value);
        }
        mutexObject.Unlock();
    }

    //�Ƴ����б��е�����Ԫ��
    void Clear()
    {
        mutexObject.Lock();
        valueVector.clear();
        mutexObject.Unlock();
    }

    //�ж��б����Ƿ����ĳԪ��
    bool  Contains(V &value)
    {
        typename std::vector<V>::iterator iter;
        bool bRet = false;

        mutexObject.Lock();
        for (iter = valueVector.begin(); iter != valueVector.end(); ++iter)
        {
            if((*iter) == value)
            {
                bRet = true;
                break;
            }
        }
        mutexObject.Unlock();

        return bRet;
    }

    //���ش��б���ָ��λ���ϵ�Ԫ��(ע��:index����0<=index<Size)
    V  Get(int index)
    {
        V tempValue = V();

        mutexObject.Lock();
        if(index >=0 && index < valueVector.size())
        {
            tempValue = valueVector[index];
        }
        mutexObject.Unlock();

        return tempValue;
    }

    //���ش��б����״γ��ֵ�ָ��Ԫ�ص���������������б�����Ԫ�أ��򷵻�-1
    int IndexOf(V &value)
    {
        typename std::vector<V>::iterator iter;
        int nRet = -1;

        mutexObject.Lock();
        for (iter = valueVector.begin(); iter != valueVector.end(); ++iter)
        {
            if((*iter) == value)
            {
                nRet = iter - valueVector.begin();
                break;
            }
        }
        mutexObject.Unlock();

        return nRet;
    }

    //�ж��б��Ƿ�Ϊ��
    bool IsEmpty()
    {
        bool bRet = false;

        mutexObject.Lock();
        bRet = valueVector.empty();
        mutexObject.Unlock();

        return bRet;
    }


    //���ش��б������һ�γ��ֵ�ָ��Ԫ�ص���������������б������������򷵻�-1��
    int LastIndexOf(V &value)
    {
        typename std::vector<V>::iterator iter;
        int nRet = -1;

        mutexObject.Lock();
        for (iter = valueVector.rbegin(); iter != valueVector.rend(); ++iter)
        {
            if((*iter) == value)
            {
                nRet = iter - valueVector.rend();
                break;
            }
        }
        mutexObject.Unlock();

        return nRet;
    }

    //�Ƴ����б���ָ��λ���ϵ�Ԫ��(ע��:index����0<=index<Size)
    void Remove(int index)
    {
        typename std::vector<V>::iterator iter;

        mutexObject.Lock();
        if(index >=0 && index < valueVector.size())
        {
            valueVector.erase(valueVector.begin() + index);
        }
        mutexObject.Unlock();
    }

    //�Ƴ����б����״γ��ֵ�ָ��Ԫ��(�������)
    bool  Remove(V &value)
    {
        typename std::vector<V>::iterator iter;
        bool bRet = false;

        mutexObject.Lock();
        for (iter = valueVector.begin(); iter != valueVector.end();)
        {
            if((*iter) == value)
            {
                valueVector.erase(iter);
                bRet = true;
                break;
            }
        }
        mutexObject.Unlock();

        return bRet;
    }

    //�Ƴ����б��г��ֵ�ָ��Ԫ��(������ڶ������ȫ���Ƴ�)
    bool  RemoveAll(V &value)
    {
        typename std::vector<V>::iterator iter;
        bool bRet = false;

        mutexObject.Lock();
        for (iter = valueVector.begin(); iter != valueVector.end();)
        {
            if((*iter) == value)
            {
                iter = valueVector.erase(iter);
                bRet = true;
            }
            else
            {
                ++iter;
            }
        }
        mutexObject.Unlock();

        return bRet;
    }


    //��ָ����Ԫ��������б���ָ��λ���ϵ�Ԫ��(ע��:index����0<=index<Size)
    void Set(int index, V &value)
    {
        mutexObject.Lock();
        if(index >=0 && index < valueVector.size())
        {
            valueVector[index] = value;
        }
        mutexObject.Unlock();
    }

    //���ش��б��е�Ԫ����
    uint32 Size()
    {
        uint32 itemCount = 0;

        mutexObject.Lock();
        itemCount = valueVector.size();
        mutexObject.Unlock();

        return itemCount;
    }

    //��˳�򷵻ذ������б�������Ԫ�ص�vector
    void ToArray(std::vector<V> &valueVec)
    {
        mutexObject.Lock();
        copy(valueVector.begin(),valueVector.end(),back_inserter(valueVec));
        mutexObject.Unlock();
    }

private:
    CMutex mutexObject;
    std::vector<V> valueVector;

private:
    DISALLOW_COPY_AND_ASSIGN(CConcurrentArrayList);
};


/*********************************
�������
**********************************/
template<typename V>  class CArrayQueue
{
public:

    CArrayQueue() {}
    virtual ~CArrayQueue()
    {
        if(!valueQueue.empty())
        {
            valueQueue.clear();
        }
    }

    //��ָ��Ԫ�ز����˫�˶��е�ĩβ
    void  Add(V &value)
    {
        valueQueue.push_back(value);
    }

    //��ָ��Ԫ�ز����˫�˶��еĿ�ͷ
    void AddFirst(V &value)
    {
        valueQueue.push_front(value);
    }

    //��ָ��Ԫ�ز����˫�˶��е�ĩβ
    void AddLast(V &value)
    {
        valueQueue.push_back(value);
    }

    //�Ӵ�˫�˶������Ƴ�����Ԫ��
    void Clear()
    {
        valueQueue.clear();
    }

    //�жϴ˶������Ƿ�����ض�Ԫ��
    bool  Contains(V &value)
    {
        typename std::deque<V>::iterator iter;
        bool bRet = false;

        for (iter = valueQueue.begin(); iter != valueQueue.end(); ++iter)
        {
            if((*iter) == value)
            {
                bRet = true;
                break;
            }
        }

        return bRet;
    }

    //��ȡ�����Ƴ���˫�˶�������ʾ�Ķ��е�ͷ(ע��:���б���ǿ�)
    V  Element()
    {
        V tempValue = V();
        if(!valueQueue.empty())
        {
            tempValue = valueQueue.front();
        }
        return tempValue;
    }

    //��ȡ�����Ƴ���˫�˶��еĵ�һ��Ԫ��(ע��:���б���ǿ�)
    V  GetFirst()
    {
        V tempValue = V();
        if(!valueQueue.empty())
        {
            tempValue = valueQueue.front();
        }
        return tempValue;
    }

    //��ȡ�����Ƴ���˫�˶��е����һ��Ԫ��(ע��:���б���ǿ�)
    V  GetLast()
    {
        V tempValue = V();
        if(!valueQueue.empty())
        {
            tempValue = valueQueue.back();
        }
        return tempValue;
    }

    //�жϴ˶����Ƿ�Ϊ��
    bool IsEmpty()
    {
        bool bRet = false;
        bRet = valueQueue.empty();
        return bRet;
    }

    //��ָ��Ԫ�ز����˫�˶��е�ĩβ
    void Offer(V &value)
    {
        valueQueue.push_back(value);
    }

    //��ָ��Ԫ�ز����˫�˶��еĿ�ͷ
    void OfferFisrt(V &value)
    {
        valueQueue.push_front(value);
    }

    //��ָ��Ԫ�ز����˫�˶��еĿ�ͷ
    void OfferLast(V &value)
    {
        valueQueue.push_back(value);
    }

    //��ȡ�����Ƴ���˫�˶�������ʾ�Ķ��е�ͷ(ע��:���б���ǿ�)
    V  Peek()
    {
        V tempValue = V();
        if(!valueQueue.empty())
        {
            tempValue = valueQueue.front();
        }
        return tempValue;
    }

    //��ȡ�����Ƴ���˫�˶��еĵ�һ��Ԫ��(ע��:���б���ǿ�)
    V  PeekFirst()
    {
        V tempValue = V();

        if(!valueQueue.empty())
        {
            tempValue = valueQueue.front();
        }

        return tempValue;
    }


    //��ȡ�����Ƴ���˫�˶��е����һ��Ԫ��(ע��:���б���ǿ�)
    V  PeekLast()
    {
        V tempValue = V();

        if(!valueQueue.empty())
        {
            tempValue = valueQueue.back();
        }

        return tempValue;
    }

    //��ȡ���Ƴ���˫�˶�������ʾ�Ķ��е�ͷ(ע��:���б���ǿ�)
    V  Poll()
    {
        V tempValue = V();

        if(!valueQueue.empty())
        {
            tempValue = valueQueue.front();
            valueQueue.pop_front();
        }

        return tempValue;
    }

    //��ȡ���Ƴ���˫�˶������ĵ�һ��Ԫ��(ע��:���б���ǿ�)
    V  PollFirst()
    {
        V tempValue = V();

        if(!valueQueue.empty())
        {
            tempValue = valueQueue.front();
            valueQueue.pop_front();
        }

        return tempValue;
    }

    //��ȡ���Ƴ���˫�˶����������һ��Ԫ��(ע��:���б���ǿ�)
    V  PollLast()
    {
        V tempValue = V();

        if(!valueQueue.empty())
        {
            tempValue = valueQueue.back();
            valueQueue.pop_back();
        }

        return tempValue;
    }

    //�Ӵ�˫�˶�������ʾ�Ķ�ջ�е���һ��Ԫ��(ע��:���б���ǿ�)
    V Pop()
    {
        V tempValue = V();

        if(!valueQueue.empty())
        {
            tempValue = valueQueue.front();
            valueQueue.pop_front();
        }

        return tempValue;
    }

    //��Ԫ�������˫�˶�������ʾ�Ķ�ջ
    void Push(V &value)
    {
        valueQueue.push_front(value);
    }

    //��ȡ���Ƴ���˫�˶�������ʾ�Ķ��е�ͷ(ע��:���б���ǿ�)
    V  Remove()
    {
        V tempValue = V();

        if(!valueQueue.empty())
        {
            tempValue = valueQueue.front();
            valueQueue.pop_front();
        }

        return tempValue;
    }

    //��ȡ���Ƴ���˫�˶������ĵ�һ��Ԫ��(ע��:���б���ǿ�)
    V  RemoveFirst()
    {
        V tempValue = V();

        if(!valueQueue.empty())
        {
            tempValue = valueQueue.front();
            valueQueue.pop_front();
        }

        return tempValue;
    }

    //��ȡ���Ƴ���˫�˶����������һ��Ԫ��(ע��:���б���ǿ�)
    V  RemoveLast()
    {
        V tempValue = V();

        if(!valueQueue.empty())
        {
            tempValue = valueQueue.back();
            valueQueue.pop_back();
        }

        return tempValue;
    }

    //�Ӵ�˫�˶������Ƴ�ָ��Ԫ��(���ȫ���Ƴ�)
    bool  Remove(V &value)
    {
        typename std::deque<V>::iterator iter;
        bool bRet = false;

        for (iter = valueQueue.begin(); iter != valueQueue.end();)
        {
            if((*iter) == value)
            {
                iter = valueQueue.erase(iter);
                bRet = true;
            }
            else
            {
                ++iter;
            }
        }

        return bRet;
    }


    //�Ƴ���˫�˶����е�һ�γ��ֵ�ָ��Ԫ��(����ͷ����β������˫�˶���ʱ)
    bool  RemoveFirstOccurrence(V &value)
    {
        typename std::deque<V>::iterator iter;
        bool bRet = false;

        for (iter = valueQueue.begin(); iter != valueQueue.end();)
        {
            if((*iter) == value)
            {
                valueQueue.erase(iter);
                bRet = true;
                break;
            }
        }

        return bRet;
    }

    //�Ƴ���˫�˶��������һ�γ��ֵ�ָ��Ԫ��(����ͷ����β������˫�˶���ʱ)
    bool  RemoveLastOccurrence(V &value)
    {
        typename std::deque<V>::iterator iter;
        bool bRet = false;

        for (iter = valueQueue.rbegin(); iter != valueQueue.rend();)
        {
            if((*iter) == value)
            {
                valueQueue.erase(iter);
                bRet = true;
                break;
            }
        }

        return bRet;
    }

    //���ش�˫�˶����е�Ԫ����
    uint32 Size()
    {
        return valueQueue.size();
    }

    //��˳�򷵻ذ������б�������Ԫ�ص�vector
    void ToArray(std::vector<V> &valueVector)
    {
        copy(valueQueue.begin(),valueQueue.end(),back_inserter(valueVector));
    }



private:
    std::deque<V> valueQueue;

private:
    DISALLOW_COPY_AND_ASSIGN(CArrayQueue);
};

/*********************************
�̲߳����������
**********************************/
template<typename V>  class CConcurrentArrayQueue
{
public:

    CConcurrentArrayQueue() {}
    virtual ~CConcurrentArrayQueue()
    {
        if(!valueQueue.empty())
        {
            valueQueue.clear();
        }
    }

    //��ָ��Ԫ�ز����˫�˶��е�ĩβ
    void  Add(V &value)
    {
        mutexObject.Lock();
        valueQueue.push_back(value);
        mutexObject.Unlock();
    }

    //��ָ��Ԫ�ز����˫�˶��еĿ�ͷ
    void AddFirst(V &value)
    {
        mutexObject.Lock();
        valueQueue.push_front(value);
        mutexObject.Unlock();
    }

    //��ָ��Ԫ�ز����˫�˶��е�ĩβ
    void AddLast(V &value)
    {
        mutexObject.Lock();
        valueQueue.push_back(value);
        mutexObject.Unlock();
    }

    //�Ӵ�˫�˶������Ƴ�����Ԫ��
    void Clear()
    {
        mutexObject.Lock();
        valueQueue.clear();
        mutexObject.Unlock();
    }

    //�жϴ˶������Ƿ�����ض�Ԫ��
    bool  Contains(V &value)
    {
        typename std::deque<V>::iterator iter;
        bool bRet = false;

        mutexObject.Lock();
        for (iter = valueQueue.begin(); iter != valueQueue.end(); ++iter)
        {
            if((*iter) == value)
            {
                bRet = true;
                break;
            }
        }
        mutexObject.Unlock();

        return bRet;
    }

    //��ȡ�����Ƴ���˫�˶�������ʾ�Ķ��е�ͷ(ע��:���б���ǿ�)
    V  Element()
    {
        V tempValue = V();

        mutexObject.Lock();
        if(!valueQueue.empty())
        {
            tempValue = valueQueue.front();
        }
        mutexObject.Unlock();

        return tempValue;
    }

    //��ȡ�����Ƴ���˫�˶��еĵ�һ��Ԫ��(ע��:���б���ǿ�)
    V  GetFirst()
    {
        V tempValue = V();

        mutexObject.Lock();
        if(!valueQueue.empty())
        {
            tempValue = valueQueue.front();
        }
        mutexObject.Unlock();

        return tempValue;
    }

    //��ȡ�����Ƴ���˫�˶��е����һ��Ԫ��(ע��:���б���ǿ�)
    V  GetLast()
    {
        V tempValue = V();

        mutexObject.Lock();
        if(!valueQueue.empty())
        {
            tempValue = valueQueue.back();
        }
        mutexObject.Unlock();

        return tempValue;
    }

    //�жϴ˶����Ƿ�Ϊ��
    bool IsEmpty()
    {
        bool bRet = false;

        mutexObject.Lock();
        bRet = valueQueue.empty();
        mutexObject.Unlock();

        return bRet;
    }

    //��ָ��Ԫ�ز����˫�˶��е�ĩβ
    void Offer(V &value)
    {
        mutexObject.Lock();
        valueQueue.push_back(value);
        mutexObject.Unlock();
    }

    //��ָ��Ԫ�ز����˫�˶��еĿ�ͷ
    void OfferFisrt(V &value)
    {
        mutexObject.Lock();
        valueQueue.push_front(value);
        mutexObject.Unlock();
    }

    //��ָ��Ԫ�ز����˫�˶��еĿ�ͷ
    void OfferLast(V &value)
    {
        mutexObject.Lock();
        valueQueue.push_back(value);
        mutexObject.Unlock();
    }

    //��ȡ�����Ƴ���˫�˶�������ʾ�Ķ��е�ͷ(ע��:���б���ǿ�)
    V  Peek()
    {
        V tempValue = V();

        mutexObject.Lock();
        if(!valueQueue.empty())
        {
            tempValue = valueQueue.front();
        }
        mutexObject.Unlock();

        return tempValue;
    }

    //��ȡ�����Ƴ���˫�˶��еĵ�һ��Ԫ��(ע��:���б���ǿ�)
    V  PeekFirst()
    {
        V tempValue = V();

        mutexObject.Lock();
        if(!valueQueue.empty())
        {
            tempValue = valueQueue.front();
        }
        mutexObject.Unlock();

        return tempValue;
    }


    //��ȡ�����Ƴ���˫�˶��е����һ��Ԫ��(ע��:���б���ǿ�)
    V  PeekLast()
    {
        V tempValue = V();

        mutexObject.Lock();
        if(!valueQueue.empty())
        {
            tempValue = valueQueue.back();
        }
        mutexObject.Unlock();

        return tempValue;
    }

    //��ȡ���Ƴ���˫�˶�������ʾ�Ķ��е�ͷ(ע��:���б���ǿ�)
    V  Poll()
    {
        V tempValue = V();

        mutexObject.Lock();
        if(!valueQueue.empty())
        {
            tempValue = valueQueue.front();
            valueQueue.pop_front();
        }
        mutexObject.Unlock();

        return tempValue;
    }

    //��ȡ���Ƴ���˫�˶������ĵ�һ��Ԫ��(ע��:���б���ǿ�)
    V  PollFirst()
    {
        V tempValue = V();

        mutexObject.Lock();
        if(!valueQueue.empty())
        {
            tempValue = valueQueue.front();
            valueQueue.pop_front();
        }
        mutexObject.Unlock();

        return tempValue;
    }

    //��ȡ���Ƴ���˫�˶����������һ��Ԫ��(ע��:���б���ǿ�)
    V  PollLast()
    {
        V tempValue = V();

        mutexObject.Lock();
        if(!valueQueue.empty())
        {
            tempValue = valueQueue.back();
            valueQueue.pop_back();
        }
        mutexObject.Unlock();

        return tempValue;
    }

    //�Ӵ�˫�˶�������ʾ�Ķ�ջ�е���һ��Ԫ��(ע��:���б���ǿ�)
    V Pop()
    {
        V tempValue = V();

        mutexObject.Lock();
        if(!valueQueue.empty())
        {
            tempValue = valueQueue.front();
            valueQueue.pop_front();
        }
        mutexObject.Unlock();

        return tempValue;
    }

    //��Ԫ�������˫�˶�������ʾ�Ķ�ջ
    void Push(V &value)
    {
        mutexObject.Lock();
        valueQueue.push_front(value);
        mutexObject.Unlock();
    }

    //��ȡ���Ƴ���˫�˶�������ʾ�Ķ��е�ͷ(ע��:���б���ǿ�)
    V  Remove()
    {
        V tempValue = V();

        mutexObject.Lock();
        if(!valueQueue.empty())
        {
            tempValue = valueQueue.front();
            valueQueue.pop_front();
        }
        mutexObject.Unlock();

        return tempValue;
    }

    //��ȡ���Ƴ���˫�˶������ĵ�һ��Ԫ��(ע��:���б���ǿ�)
    V  RemoveFirst()
    {
        V tempValue = V();

        mutexObject.Lock();
        if(!valueQueue.empty())
        {
            tempValue = valueQueue.front();
            valueQueue.pop_front();
        }
        mutexObject.Unlock();

        return tempValue;
    }

    //��ȡ���Ƴ���˫�˶����������һ��Ԫ��(ע��:���б���ǿ�)
    V  RemoveLast()
    {
        V tempValue = V();

        mutexObject.Lock();
        if(!valueQueue.empty())
        {
            tempValue = valueQueue.back();
            valueQueue.pop_back();
        }
        mutexObject.Unlock();

        return tempValue;
    }

    //�Ӵ�˫�˶������Ƴ�ָ��Ԫ��(���ȫ���Ƴ�)
    bool  Remove(V &value)
    {
        typename std::deque<V>::iterator iter;
        bool bRet = false;

        mutexObject.Lock();
        for (iter = valueQueue.begin(); iter != valueQueue.end();)
        {
            if((*iter) == value)
            {
                iter = valueQueue.erase(iter);
                bRet = true;
            }
            else
            {
                ++iter;
            }
        }
        mutexObject.Unlock();

        return bRet;
    }


    //�Ƴ���˫�˶����е�һ�γ��ֵ�ָ��Ԫ��(����ͷ����β������˫�˶���ʱ)
    bool  RemoveFirstOccurrence(V &value)
    {
        typename std::deque<V>::iterator iter;
        bool bRet = false;

        mutexObject.Lock();
        for (iter = valueQueue.begin(); iter != valueQueue.end();)
        {
            if((*iter) == value)
            {
                valueQueue.erase(iter);
                bRet = true;
                break;
            }
        }
        mutexObject.Unlock();

        return bRet;
    }

    //�Ƴ���˫�˶��������һ�γ��ֵ�ָ��Ԫ��(����ͷ����β������˫�˶���ʱ)
    bool  RemoveLastOccurrence(V &value)
    {
        typename std::deque<V>::iterator iter;
        bool bRet = false;

        mutexObject.Lock();
        for (iter = valueQueue.rbegin(); iter != valueQueue.rend();)
        {
            if((*iter) == value)
            {
                valueQueue.erase(iter);
                bRet = true;
                break;
            }
        }
        mutexObject.Unlock();

        return bRet;
    }

    //���ش�˫�˶����е�Ԫ����
    uint32 Size()
    {
        uint32 itemCount = 0;

        mutexObject.Lock();
        itemCount = valueQueue.size();
        mutexObject.Unlock();

        return itemCount;
    }

    //��˳�򷵻ذ������б�������Ԫ�ص�vector
    void ToArray(std::vector<V> &valueVector)
    {
        mutexObject.Lock();
        copy(valueQueue.begin(),valueQueue.end(),back_inserter(valueVector));
        mutexObject.Unlock();
    }

private:
    CMutex mutexObject;
    std::deque<V> valueQueue;

private:
    DISALLOW_COPY_AND_ASSIGN(CConcurrentArrayQueue);
};


/*********************************
���Ӷ���
**********************************/
template<typename V>  class CLinkedList
{
public:

    CLinkedList() {}
    virtual ~CLinkedList()
    {
        if(!valueList.empty())
        {
            valueList.clear();
        }
    }

    //��ָ��Ԫ�ز���������ĩβ
    void  Add(V &value)
    {
        valueList.push_back(value);
    }

    //��ָ��Ԫ�ز���������ָ��λ��(ע�⣺index���� 0<=index<size);
    void  Add(int index, V &value)
    {
        typename std::list<V>::iterator iter;

        if(index >=0 && index < valueList.size())
        {
            iter = valueList.begin();
            advance(iter, index);
            valueList.insert(iter,value);
        }
    }

    //��ָ��Ԫ�ز��������Ŀ�ͷ
    void AddFirst(V &value)
    {
        valueList.push_front(value);
    }

    //��ָ��Ԫ�ز���������ĩβ
    void AddLast(V &value)
    {
        valueList.push_back(value);
    }

    //�Ӵ��������Ƴ�����Ԫ��
    void Clear()
    {
        valueList.clear();
    }

    //�жϴ��������Ƿ�����ض�Ԫ��
    bool  Contains(V &value)
    {
        typename std::list<V>::iterator iter;
        bool bRet = false;

        for (iter = valueList.begin(); iter != valueList.end(); ++iter)
        {
            if((*iter) == value)
            {
                bRet = true;
                break;
            }
        }

        return bRet;
    }

    //��ȡ�����Ƴ��������ͷ(ע��:�������ǿ�)
    V  Element()
    {
        V tempValue = V();

        if(!valueList.empty())
        {
            tempValue = valueList.front();
        }

        return tempValue;
    }

    //���ش�������ָ��λ�ô���Ԫ��(ע�⣺index����0<=index<size)
    V  Get(int index)
    {
        typename std::list<V>::iterator iter;
        V tempValue = V();

        if(index >=0 && index < valueList.size())
        {
            iter = valueList.begin();
            advance(iter, index);
            tempValue = (*iter);
        }

        return tempValue;
    }

    //��ȡ�����Ƴ�������ĵ�һ��Ԫ��(ע��:�������ǿ�)
    V  GetFirst()
    {
        V tempValue = V();

        if(!valueList.empty())
        {
            tempValue = valueList.front();
        }

        return tempValue;
    }

    //��ȡ�����Ƴ�����������һ��Ԫ��(ע��:���б���ǿ�)
    V  GetLast()
    {
        V tempValue = V();

        if(!valueList.empty())
        {
            tempValue = valueList.back();
        }

        return tempValue;
    }

    //���ش��������״γ��ֵ�ָ��Ԫ�ص���������������б�����Ԫ�أ��򷵻�-1
    int IndexOf(V &value)
    {
        typename std::list<V>::iterator iter;
        int nRet = -1;
        int i = 0;

        for (iter = valueList.begin(); iter != valueList.end(); ++iter)
        {
            if((*iter) == value)
            {
                nRet = i;
                break;
            }
            i++;
        }

        return nRet;
    }

    //���ش������������ֵ�ָ��Ԫ�ص���������������б�����Ԫ�أ��򷵻�-1
    int LastIndexOf(V &value)
    {
        typename std::list<V>::iterator iter;
        int nRet = -1;
        int i = 0;

        for (iter = valueList.rbegin(); iter != valueList.rend(); ++iter)
        {
            if((*iter) == value)
            {
                nRet = valueList.size() -1 - i;
                break;
            }
            i++;
        }

        return nRet;
    }

    //�жϴ˶����Ƿ�Ϊ��
    bool IsEmpty()
    {
        bool bRet = false;
        bRet = valueList.empty();
        return bRet;
    }

    //��ָ��Ԫ�ز���������ĩβ
    void Offer(V &value)
    {
        valueList.push_back(value);
    }

    //��ָ��Ԫ�ز��������Ŀ�ͷ
    void OfferFisrt(V &value)
    {
        valueList.push_front(value);
    }

    //��ָ��Ԫ�ز��������Ŀ�ͷ
    void OfferLast(V &value)
    {
        valueList.push_back(value);
    }

    //��ȡ�����Ƴ��������ͷ(ע��:�������ǿ�)
    V  Peek()
    {
        V tempValue = V();

        if(!valueList.empty())
        {
            tempValue = valueList.front();
        }

        return tempValue;
    }

    //��ȡ�����Ƴ�������ĵ�һ��Ԫ��(ע��:�������ǿ�)
    V  PeekFirst()
    {
        V tempValue = V();

        if(!valueList.empty())
        {
            tempValue = valueList.front();
        }

        return tempValue;
    }


    //��ȡ�����Ƴ�����������һ��Ԫ��(ע��:�������ǿ�)
    V  PeekLast()
    {
        V tempValue = V();

        if(!valueList.empty())
        {
            tempValue = valueList.back();
        }

        return tempValue;
    }

    //��ȡ���Ƴ��������ͷ(ע��:�������ǿ�)
    V  Poll()
    {
        V tempValue = V();

        if(!valueList.empty())
        {
            tempValue = valueList.front();
            valueList.pop_front();
        }

        return tempValue;
    }

    //��ȡ���Ƴ����������ĵ�һ��Ԫ��(ע��:�������ǿ�)
    V  PollFirst()
    {
        V tempValue = V();

        if(!valueList.empty())
        {
            tempValue = valueList.front();
            valueList.pop_front();
        }

        return tempValue;
    }

    //��ȡ���Ƴ��������������һ��Ԫ��(ע��:�������ǿ�)
    V  PollLast()
    {
        V tempValue = V();

        if(!valueList.empty())
        {
            tempValue = valueList.back();
            valueList.pop_back();
        }

        return tempValue;
    }

    //�Ӵ���������ʾ�Ķ�ջ�е���һ��Ԫ��(ע��:�������ǿ�)
    V Pop()
    {
        V tempValue = V();

        if(!valueList.empty())
        {
            tempValue = valueList.front();
            valueList.pop_front();
        }

        return tempValue;
    }

    //��Ԫ���������������ʾ�Ķ�ջ
    void Push(V &value)
    {
        valueList.push_front(value);
    }

    //��ȡ���Ƴ�����������ʾ�Ķ��е�ͷ(ע��:�������ǿ�)
    V  Remove()
    {
        V tempValue = V();

        if(!valueList.empty())
        {
            tempValue = valueList.front();
            valueList.pop_front();
        }

        return tempValue;
    }

    //��ȡ���Ƴ����������ĵ�һ��Ԫ��(ע��:�������ǿ�)
    V  RemoveFirst()
    {
        V tempValue = V();

        if(!valueList.empty())
        {
            tempValue = valueList.front();
            valueList.pop_front();
        }

        return tempValue;
    }

    //��ȡ���Ƴ��������������һ��Ԫ��(ע��:�������ǿ�)
    V  RemoveLast()
    {
        V tempValue = V();

        if(!valueList.empty())
        {
            tempValue = valueList.back();
            valueList.pop_back();
        }

        return tempValue;
    }

    //�Ӵ��������Ƴ�ָ��Ԫ��(���ȫ���Ƴ�)
    bool  Remove(V &value)
    {
        typename std::list<V>::iterator iter;
        bool bRet = false;

        for (iter = valueList.begin(); iter != valueList.end();)
        {
            if((*iter) == value)
            {
                iter = valueList.erase(iter);
                bRet = true;
            }
            else
            {
                ++iter;
            }
        }

        return bRet;
    }


    //�Ƴ��������е�һ�γ��ֵ�ָ��Ԫ��(����ͷ����β����������ʱ)
    bool  RemoveFirstOccurrence(V &value)
    {
        typename std::list<V>::iterator iter;
        bool bRet = false;

        for (iter = valueList.begin(); iter != valueList.end();)
        {
            if((*iter) == value)
            {
                valueList.erase(iter);
                bRet = true;
                break;
            }
        }

        return bRet;
    }

    //�Ƴ������������һ�γ��ֵ�ָ��Ԫ��(����ͷ����β����������ʱ)
    bool  RemoveLastOccurrence(V &value)
    {
        typename std::list<V>::iterator iter;
        bool bRet = false;

        for (iter = valueList.rbegin(); iter != valueList.rend();)
        {
            if((*iter) == value)
            {
                valueList.erase(iter);
                bRet = true;
                break;
            }
        }

        return bRet;
    }

    //�����б���ָ��λ�õ�Ԫ���滻Ϊָ����Ԫ�أ�������ǰ��Ԫ��(ע�⣺index���� 0<=index<size);
    V  Set(int index, V &value)
    {
        typename std::list<V>::iterator iter;
        V tempValue = V();

        if(index >=0 && index < valueList.size())
        {
            iter = valueList.begin();
            advance(iter, index);
            tempValue = (*iter);
            (*iter) = value;
        }

        return tempValue;
    }


    //���ش������е�Ԫ����
    uint32 Size()
    {
        return valueList.size();
    }

    //��˳�򷵻ذ������б�������Ԫ�ص�vector
    void ToArray(std::vector<V> &valueVector)
    {
        copy(valueList.begin(),valueList.end(),back_inserter(valueVector));
    }



private:
    std::list<V> valueList;

private:
    DISALLOW_COPY_AND_ASSIGN(CLinkedList);
};


/*********************************
�̲߳������Ӷ���
**********************************/
template<typename V>  class CConcurrentLinkedList
{
public:

    CConcurrentLinkedList() {}
    virtual ~CConcurrentLinkedList()
    {
        if(!valueList.empty())
        {
            valueList.clear();
        }
    }

    //��ָ��Ԫ�ز���������ĩβ
    void  Add(V &value)
    {
        mutexObject.Lock();
        valueList.push_back(value);
        mutexObject.Unlock();
    }

    //��ָ��Ԫ�ز���������ָ��λ��(ע�⣺index���� 0<=index<size);
    void  Add(int index, V &value)
    {
        typename std::list<V>::iterator iter;

        mutexObject.Lock();
        if(index >=0 && index < valueList.size())
        {
            iter = valueList.begin();
            advance(iter, index);
            valueList.insert(iter,value);
        }
        mutexObject.Unlock();
    }

    //��ָ��Ԫ�ز��������Ŀ�ͷ
    void AddFirst(V &value)
    {
        mutexObject.Lock();
        valueList.push_front(value);
        mutexObject.Unlock();
    }

    //��ָ��Ԫ�ز���������ĩβ
    void AddLast(V &value)
    {
        mutexObject.Lock();
        valueList.push_back(value);
        mutexObject.Unlock();
    }

    //�Ӵ��������Ƴ�����Ԫ��
    void Clear()
    {
        mutexObject.Lock();
        valueList.clear();
        mutexObject.Unlock();
    }

    //�жϴ��������Ƿ�����ض�Ԫ��
    bool  Contains(V &value)
    {
        typename std::deque<V>::iterator iter;
        bool bRet = false;

        mutexObject.Lock();
        for (iter = valueList.begin(); iter != valueList.end(); ++iter)
        {
            if((*iter) == value)
            {
                bRet = true;
                break;
            }
        }
        mutexObject.Unlock();

        return bRet;
    }

    //��ȡ�����Ƴ��������ͷ(ע��:�������ǿ�)
    V  Element()
    {
        V tempValue = V();

        mutexObject.Lock();
        if(!valueList.empty())
        {
            tempValue = valueList.front();
        }
        mutexObject.Unlock();

        return tempValue;
    }

    //���ش�������ָ��λ�ô���Ԫ��(ע�⣺index����0<=index<size)
    V  Get(int index)
    {
        typename std::list<V>::iterator iter;
        V tempValue = V();

        mutexObject.Lock();
        if(index >=0 && index < valueList.size())
        {
            iter = valueList.begin();
            advance(iter, index);
            tempValue = (*iter);
        }
        mutexObject.Unlock();

        return tempValue;
    }

    //��ȡ�����Ƴ�������ĵ�һ��Ԫ��(ע��:�������ǿ�)
    V  GetFirst()
    {
        V tempValue = V();

        mutexObject.Lock();
        if(!valueList.empty())
        {
            tempValue = valueList.front();
        }
        mutexObject.Unlock();

        return tempValue;
    }

    //��ȡ�����Ƴ�����������һ��Ԫ��(ע��:���б���ǿ�)
    V  GetLast()
    {
        V tempValue = V();

        mutexObject.Lock();
        if(!valueList.empty())
        {
            tempValue = valueList.back();
        }
        mutexObject.Unlock();

        return tempValue;
    }

    //���ش��������״γ��ֵ�ָ��Ԫ�ص���������������б�����Ԫ�أ��򷵻�-1
    int IndexOf(V &value)
    {
        typename std::list<V>::iterator iter;
        int nRet = -1;
        int i = 0;

        mutexObject.Lock();
        for (iter = valueList.begin(); iter != valueList.end(); ++iter)
        {
            if((*iter) == value)
            {
                nRet = i;
                break;
            }
            i++;
        }
        mutexObject.Unlock();

        return nRet;
    }

    //���ش������������ֵ�ָ��Ԫ�ص���������������б�����Ԫ�أ��򷵻�-1
    int LastIndexOf(V &value)
    {
        typename std::list<V>::iterator iter;
        int nRet = -1;
        int i = 0;

        mutexObject.Lock();
        for (iter = valueList.rbegin(); iter != valueList.rend(); ++iter)
        {
            if((*iter) == value)
            {
                nRet = valueList.size() -1 - i;
                break;
            }
            i++;
        }
        mutexObject.Unlock();

        return nRet;
    }

    //�жϴ˶����Ƿ�Ϊ��
    bool IsEmpty()
    {
        bool bRet = false;

        mutexObject.Lock();
        bRet = valueList.empty();
        mutexObject.Unlock();

        return bRet;
    }

    //��ָ��Ԫ�ز���������ĩβ
    void Offer(V &value)
    {
        mutexObject.Lock();
        valueList.push_back(value);
        mutexObject.Unlock();
    }

    //��ָ��Ԫ�ز��������Ŀ�ͷ
    void OfferFisrt(V &value)
    {
        mutexObject.Lock();
        valueList.push_front(value);
        mutexObject.Unlock();
    }

    //��ָ��Ԫ�ز��������Ŀ�ͷ
    void OfferLast(V &value)
    {
        mutexObject.Lock();
        valueList.push_back(value);
        mutexObject.Unlock();
    }

    //��ȡ�����Ƴ��������ͷ(ע��:�������ǿ�)
    V  Peek()
    {
        V tempValue = V();

        mutexObject.Lock();
        if(!valueList.empty())
        {
            tempValue = valueList.front();
        }
        mutexObject.Unlock();

        return tempValue;
    }

    //��ȡ�����Ƴ�������ĵ�һ��Ԫ��(ע��:�������ǿ�)
    V  PeekFirst()
    {
        V tempValue = V();

        mutexObject.Lock();
        if(!valueList.empty())
        {
            tempValue = valueList.front();
        }
        mutexObject.Unlock();

        return tempValue;
    }


    //��ȡ�����Ƴ�����������һ��Ԫ��(ע��:�������ǿ�)
    V  PeekLast()
    {
        V tempValue = V();

        mutexObject.Lock();
        if(!valueList.empty())
        {
            tempValue = valueList.back();
        }
        mutexObject.Unlock();

        return tempValue;
    }

    //��ȡ���Ƴ��������ͷ(ע��:�������ǿ�)
    V  Poll()
    {
        V tempValue = V();

        mutexObject.Lock();
        if(!valueList.empty())
        {
            tempValue = valueList.front();
            valueList.pop_front();
        }
        mutexObject.Unlock();

        return tempValue;
    }

    //��ȡ���Ƴ����������ĵ�һ��Ԫ��(ע��:�������ǿ�)
    V  PollFirst()
    {
        V tempValue = V();

        mutexObject.Lock();
        if(!valueList.empty())
        {
            tempValue = valueList.front();
            valueList.pop_front();
        }
        mutexObject.Unlock();

        return tempValue;
    }

    //��ȡ���Ƴ��������������һ��Ԫ��(ע��:�������ǿ�)
    V  PollLast()
    {
        V tempValue = V();

        mutexObject.Lock();
        if(!valueList.empty())
        {
            tempValue = valueList.back();
            valueList.pop_back();
        }
        mutexObject.Unlock();

        return tempValue;
    }

    //�Ӵ���������ʾ�Ķ�ջ�е���һ��Ԫ��(ע��:�������ǿ�)
    V Pop()
    {
        V tempValue = V();

        mutexObject.Lock();
        if(!valueList.empty())
        {
            tempValue = valueList.front();
            valueList.pop_front();
        }
        mutexObject.Unlock();

        return tempValue;
    }

    //��Ԫ���������������ʾ�Ķ�ջ
    void Push(V &value)
    {
        mutexObject.Lock();
        valueList.push_front(value);
        mutexObject.Unlock();
    }

    //��ȡ���Ƴ�����������ʾ�Ķ��е�ͷ(ע��:�������ǿ�)
    V  Remove()
    {
        V tempValue = V();

        mutexObject.Lock();
        if(!valueList.empty())
        {
            tempValue = valueList.front();
            valueList.pop_front();
        }
        mutexObject.Unlock();

        return tempValue;
    }

    //��ȡ���Ƴ����������ĵ�һ��Ԫ��(ע��:�������ǿ�)
    V  RemoveFirst()
    {
        V tempValue = V();

        mutexObject.Lock();
        if(!valueList.empty())
        {
            tempValue = valueList.front();
            valueList.pop_front();
        }
        mutexObject.Unlock();

        return tempValue;
    }

    //��ȡ���Ƴ��������������һ��Ԫ��(ע��:�������ǿ�)
    V  RemoveLast()
    {
        V tempValue = V();

        mutexObject.Lock();
        if(!valueList.empty())
        {
            tempValue = valueList.back();
            valueList.pop_back();
        }
        mutexObject.Unlock();

        return tempValue;
    }

    //�Ӵ��������Ƴ�ָ��Ԫ��(���ȫ���Ƴ�)
    bool  Remove(V &value)
    {
        typename std::list<V>::iterator iter;
        bool bRet = false;

        mutexObject.Lock();

        for (iter = valueList.begin(); iter != valueList.end();)
        {
            if((*iter) == value)
            {
                iter = valueList.erase(iter);
                bRet = true;
            }
            else
            {
                ++iter;
            }
        }
        mutexObject.Unlock();

        return bRet;
    }


    //�Ƴ��������е�һ�γ��ֵ�ָ��Ԫ��(����ͷ����β����������ʱ)
    bool  RemoveFirstOccurrence(V &value)
    {
        typename std::list<V>::iterator iter;
        bool bRet = false;

        mutexObject.Lock();
        for (iter = valueList.begin(); iter != valueList.end();)
        {
            if((*iter) == value)
            {
                valueList.erase(iter);
                bRet = true;
                break;
            }
        }
        mutexObject.Unlock();

        return bRet;
    }

    //�Ƴ������������һ�γ��ֵ�ָ��Ԫ��(����ͷ����β����������ʱ)
    bool  RemoveLastOccurrence(V &value)
    {
        typename std::list<V>::iterator iter;
        bool bRet = false;

        mutexObject.Lock();
        for (iter = valueList.rbegin(); iter != valueList.rend();)
        {
            if((*iter) == value)
            {
                valueList.erase(iter);
                bRet = true;
                break;
            }
        }
        mutexObject.Unlock();

        return bRet;
    }

    //�����б���ָ��λ�õ�Ԫ���滻Ϊָ����Ԫ�أ�����ԭ����Ԫ��(ע�⣺index���� 0<=index<size);
    V  Set(int index, V &value)
    {
        typename std::list<V>::iterator iter;
        V tempValue = V();

        mutexObject.Lock();
        if(index >=0 && index < valueList.size())
        {
            iter = valueList.begin();
            advance(iter, index);
            tempValue = (*iter);
            (*iter) = value;
        }
        mutexObject.Unlock();

        return tempValue;
    }


    //���ش������е�Ԫ����
    uint32 Size()
    {
        uint32 itemCount = 0;

        mutexObject.Lock();
        itemCount = valueList.size();
        mutexObject.Unlock();

        return itemCount;
    }

    //��˳�򷵻ذ������б�������Ԫ�ص�vector
    void ToArray(std::vector<V> &valueVector)
    {
        mutexObject.Lock();
        copy(valueList.begin(),valueList.end(),back_inserter(valueVector));
        mutexObject.Unlock();
    }

private:
    CMutex mutexObject;
    std::list<V> valueList;

private:
    DISALLOW_COPY_AND_ASSIGN(CConcurrentLinkedList);
};


/**************************************************************************
MAP����(KEYʹ��HASH�洢)
**************************************************************************/
template <typename K, typename V>  class CHashMap
{
public:
    CHashMap(void) {}
    ~CHashMap(void)
    {
        if(!keyValueHashMap.empty())
        {
            keyValueHashMap.clear();
        }
    }

    //ɾ�����м��ͼ�ֵ
    void Clear()
    {
        keyValueHashMap.clear();
    }

    //�ж�MAP���Ƿ�����ض��ļ�
    bool ContainsKey(K &key)
    {
        bool bRet = false;

        if(keyValueHashMap.find(key) != keyValueHashMap.end())
        {
            bRet = true;
        }

        return bRet;
    }

    //�ж�MAP���Ƿ�����ض���ֵ
    bool ContainsValue(V &value)
    {
        typename boost::unordered_map<K, V>::iterator iter;
        bool bRet = false;

        for(iter = keyValueHashMap.begin(); iter != keyValueHashMap.end(); ++iter)
        {
            if(iter->second == value)
            {
                bRet = true;
                break;
            }
        }

        return bRet;
    }

    //�����ض�����ֵ(ע�⣺Ҫ��ȷ�ϸü��Ƿ����)
    V Get(K &key)
    {
        typename boost::unordered_map<K, V>::iterator iter;
        V value = V();

        iter = keyValueHashMap.find(key);
        if( iter != keyValueHashMap.end())
        {
            value = iter->second;
        }

        return value;
    }

    //�жϴ�MAP�Ƿ�Ϊ��
    bool IsEmpty()
    {
        return  keyValueHashMap.empty();
    }

    //�������м���VECTOR��
    void Keys(std::vector<K> &keyVector)
    {
        typename boost::unordered_map<K, V>::iterator iter;

        for(iter = keyValueHashMap.begin(); iter != keyValueHashMap.end(); ++iter)
        {
            keyVector.push_back(iter->first);
        }
    }

    //���ָ���ļ�ֵ(KEY�����ڻ���ӣ��������滻)
    void Put(K &key, V &value)
    {
        keyValueHashMap[key] = value;
    }

    //�Ƴ��ض��ļ�ֵ
    void Remove(K &key)
    {
        keyValueHashMap.erase(key);
    }

    //�Ƴ��ض��ļ�ֵ(����ֵ����Ȳ��Ƴ�)
    bool Remove(K &key, V &value)
    {
        bool bRet = false;

        if(keyValueHashMap.find(key) != keyValueHashMap.end())
        {
            if(keyValueHashMap[key] == value)
            {
                keyValueHashMap.erase(key);
                bRet = true;
            }
        }

        return bRet;
    }

    //�滻ָ������ֵ(KEY�����ڲ��滻���������򲻴���)
    void Replace(K &key, V &value)
    {
        if(keyValueHashMap.find(key) != keyValueHashMap.end())
        {
            keyValueHashMap[key] = value;
        }
    }

    //�滻ָ������ֵ(KEY��VALUE�����ڲ��滻���������򲻴���)
    bool Replace(K &key, V &oldValue, V &newValue)
    {
        bool bRet = false;

        if(keyValueHashMap.find(key) != keyValueHashMap.end())
        {
            if(keyValueHashMap[key] == oldValue)
            {
                keyValueHashMap[key] = newValue;
                bRet = true;
            }
        }

        return bRet;
    }

    //����MAP�Ĵ�С
    uint32   Size()
    {
        return keyValueHashMap.size();
    }

    //��������ֵ��VECTOR��
    void Values(std::vector<V> &valueVector)
    {
        typename boost::unordered_map<K, V>::iterator iter;

        for(iter = keyValueHashMap.begin(); iter != keyValueHashMap.end(); ++iter)
        {
            valueVector.push_back(iter->second);
        }
    }

    //��MAP���Ƴ���ֵ��(��ֵ���ʱ�Ƴ�)
    void RemoveValue(V &value)
    {
        typename boost::unordered_map<K, V>::iterator iter;

        for(iter = keyValueHashMap.begin(); iter != keyValueHashMap.end();)
        {
            if(iter->second == value)
            {
                keyValueHashMap.erase(iter++);
            }
            else
            {
                ++iter;
            }
        }
    }

private:
	boost::unordered_map<K, V> keyValueHashMap;

private:
    DISALLOW_COPY_AND_ASSIGN(CHashMap);
};


/***********************************************************************************
�߳�ͬ��HASH_MAP(KEYʹ��HASH�洢)
***********************************************************************************/
template <typename K, typename V>  class CConcurrentHashMap
{
public:
    CConcurrentHashMap(void) {}
    ~CConcurrentHashMap(void)
    {
        if(!keyValueHashMap.empty())
        {
            keyValueHashMap.clear();
        }
    }

    //ɾ�����м��ͼ�ֵ
    void Clear()
    {
        mutexObject.Lock();
        keyValueHashMap.clear();
        mutexObject.Unlock();
    }

    //�ж�MAP���Ƿ�����ض��ļ�
    bool ContainsKey(K &key)
    {
        bool bRet = false;

        mutexObject.Lock();
        if(keyValueHashMap.find(key) != keyValueHashMap.end())
        {
            bRet = true;
        }
        mutexObject.Unlock();

        return bRet;
    }

    //�ж�MAP���Ƿ�����ض���ֵ
    bool ContainsValue(V &value)
    {
        typename boost::unordered_map<K, V>::iterator iter;
        bool bRet = false;

        mutexObject.Lock();
        for(iter = keyValueHashMap.begin(); iter != keyValueHashMap.end(); ++iter)
        {
            if(iter->second == value)
            {
                bRet = true;
                break;
            }
        }
        mutexObject.Unlock();

        return bRet;
    }

    //�����ض�����ֵ,�������ڣ��򷵻�0(ע�⣺Ҫ��ȷ�ϸü��Ƿ����)
    V Get(K &key)
    {
        typename boost::unordered_map<K, V>::iterator iter;
        V value = V();

        mutexObject.Lock();
        iter = keyValueHashMap.find(key);
        if( iter != keyValueHashMap.end())
        {
            value = iter->second;
        }
        mutexObject.Unlock();

        return value;
    }

    //�жϴ�MAP�Ƿ�Ϊ��
    bool IsEmpty()
    {
        bool bRet = false;

        mutexObject.Lock();
        bRet = keyValueHashMap.empty();
        mutexObject.Unlock();

        return bRet;
    }

    //�������м���VECTOR��
    void Keys(std::vector<K> &keyVector)
    {
        typename boost::unordered_map<K, V>::iterator iter;

        mutexObject.Lock();
        for(iter = keyValueHashMap.begin(); iter != keyValueHashMap.end(); ++iter)
        {
            keyVector.push_back(iter->first);
        }
        mutexObject.Unlock();
    }

    //���ָ���ļ�ֵ(KEY�����ڻ���ӣ��������滻)
    void Put(K &key, V &value)
    {
        mutexObject.Lock();
        keyValueHashMap[key] = value;
        mutexObject.Unlock();
    }

    //�Ƴ��ض��ļ�ֵ
    void Remove(K &key)
    {
        mutexObject.Lock();
        keyValueHashMap.erase(key);
        mutexObject.Unlock();
    }

    //�Ƴ��ض��ļ�ֵ(����ֵ����Ȳ��Ƴ�)
    bool Remove(K &key, V &value)
    {
        bool bRet = false;

        mutexObject.Lock();
        if(keyValueHashMap.find(key) != keyValueHashMap.end())
        {
            if(keyValueHashMap[key] == value)
            {
                keyValueHashMap.erase(key);
                bRet = true;
            }
        }
        mutexObject.Unlock();

        return bRet;
    }

    //�滻ָ������ֵ(KEY�����ڲ��滻���������򲻴���)
    void Replace(K &key, V &value)
    {
        mutexObject.Lock();
        if(keyValueHashMap.find(key) != keyValueHashMap.end())
        {
            keyValueHashMap[key] = value;
        }
        mutexObject.Unlock();
    }

    //�滻ָ������ֵ(KEY��VALUE�����ڲ��滻���������򲻴���)
    bool Replace(K &key, V &oldValue, V &newValue)
    {
        bool bRet = false;

        mutexObject.Lock();
        if(keyValueHashMap.find(key) != keyValueHashMap.end())
        {
            if(keyValueHashMap[key] == oldValue)
            {
                keyValueHashMap[key] = newValue;
                bRet = true;
            }
        }
        mutexObject.Unlock();

        return bRet;
    }

    //����MAP�Ĵ�С
    uint32   Size()
    {
        uint32 nCount = 0;

        mutexObject.Lock();
        nCount = keyValueHashMap.size();
        mutexObject.Unlock();

        return nCount;
    }

    //��������ֵ��VECTOR��
    void Values(std::vector<V> &valueVector)
    {
        typename boost::unordered_map<K, V>::iterator iter;

        mutexObject.Lock();
        for(iter = keyValueHashMap.begin(); iter != keyValueHashMap.end(); ++iter)
        {
            valueVector.push_back(iter->second);
        }
        mutexObject.Unlock();
    }

    //��MAP���Ƴ���ֵ��(��ֵ���ʱ�Ƴ�)
    void RemoveValue(V &value)
    {
        typename boost::unordered_map<K, V>::iterator iter;

        mutexObject.Lock();
        for(iter = keyValueHashMap.begin(); iter != keyValueHashMap.end();)
        {
            if(iter->second == value)
            {
                keyValueHashMap.erase(iter++);
            }
            else
            {
                ++iter;
            }
        }
        mutexObject.Unlock();
    }

private:
	boost::unordered_map<K, V> keyValueHashMap;
    CMutex mutexObject;

private:
    DISALLOW_COPY_AND_ASSIGN(CConcurrentHashMap);
};

/********************************************************************************
MAP(KEYʹ�ú�����洢)
********************************************************************************/
template <class K, class V>  class CMap
{
public:
    CMap(void) {}
    virtual ~CMap(void)
    {
        if(!keyValueMap.empty())
        {
            keyValueMap.clear();
        }
    }

    //ɾ�����м��ͼ�ֵ
    void Clear()
    {
        keyValueMap.clear();
    }

    //�ж�MAP���Ƿ�����ض��ļ�
    bool ContainsKey(K &key)
    {
        bool bRet = false;

        if(keyValueMap.find(key) != keyValueMap.end())
        {
            bRet = true;
        }

        return bRet;
    }

    //�ж�MAP���Ƿ�����ض���ֵ
    bool ContainsValue(V &value)
    {
        typename std::map<K, V>::iterator iter;
        bool bRet = false;

        for(iter = keyValueMap.begin(); iter != keyValueMap.end(); ++iter)
        {
            if(iter->second == value)
            {
                bRet = true;
                break;
            }
        }

        return bRet;
    }

    //�����ض�����ֵ,�������ڣ��򷵻�0(ע�⣺Ҫ��ȷ�ϸü��Ƿ����)
    V Get(K &key)
    {
        V value = V();

        if(keyValueMap.find(key) != keyValueMap.end())
        {
            value = keyValueMap[key];
        }

        return value;
    }

    //�жϴ�MAP�Ƿ�Ϊ��
    bool IsEmpty()
    {
        bool bRet = false;
        bRet = keyValueMap.empty();
        return bRet;
    }

    //�������м���VECTOR��
    void Keys(std::vector<K> &keyVector)
    {
        typename std::map<K, V>::iterator iter;

        for(iter = keyValueMap.begin(); iter != keyValueMap.end(); ++iter)
        {
            keyVector.push_back(iter->first);
        }
    }

    //���ָ���ļ�ֵ(KEY�����ڻ���ӣ��������滻)
    void Put(K &key, V &value)
    {
        keyValueMap[key] = value;
    }

    //�Ƴ��ض��ļ�ֵ
    void Remove(K &key)
    {
        keyValueMap.erase(key);
    }

    //�Ƴ��ض��ļ�ֵ(����ֵ����Ȳ��Ƴ�)
    bool Remove(K &key, V &value)
    {
        bool bRet = false;

        if(keyValueMap.find(key) != keyValueMap.end())
        {
            if(keyValueMap[key] == value)
            {
                keyValueMap.erase(key);
                bRet = true;
            }
        }

        return bRet;
    }

    //�滻ָ������ֵ(KEY�����ڲ��滻���������򲻴���)
    void Replace(K &key, V &value)
    {
        if(keyValueMap.find(key) != keyValueMap.end())
        {
            keyValueMap[key] = value;
        }
    }

    //�滻ָ������ֵ(KEY��VALUE�����ڲ��滻���������򲻴���)
    bool Replace(K &key, V &oldValue, V &newValue)
    {
        bool bRet = false;

        if(keyValueMap.find(key) != keyValueMap.end())
        {
            if(keyValueMap[key] == oldValue)
            {
                keyValueMap[key] = newValue;
                bRet = true;
            }
        }

        return bRet;
    }

    //����MAP�Ĵ�С
    uint32   Size()
    {
        return keyValueMap.size();
    }

    //��������ֵ��VECTOR��
    void Values(std::vector<V> &valueVector)
    {
        typename std::map<K, V>::iterator iter;

        for(iter = keyValueMap.begin(); iter != keyValueMap.end(); ++iter)
        {
            valueVector.push_back(iter->second);
        }
    }

    //��MAP���Ƴ���ֵ��(��ֵ���ʱ�Ƴ�)
    void RemoveValue(V &value)
    {
        typename std::map<K, V>::iterator iter;

        for(iter = keyValueMap.begin(); iter != keyValueMap.end(); ++iter)
        {
            if(iter->second == value)
            {
                keyValueMap.erase(iter);
                break;
            }
        }
    }


private:
    std::map<K, V> keyValueMap;

private:
    DISALLOW_COPY_AND_ASSIGN(CMap);
};

/********************************************************************************
�߳�ͬ��MAP(KEYʹ�ú�����洢)
********************************************************************************/
template <class K, class V>  class CConcurrentMap
{
public:
    CConcurrentMap(void) {}
    virtual ~CConcurrentMap(void)
    {
        if(!keyValueMap.empty())
        {
            keyValueMap.clear();
        }
    }

    //ɾ�����м��ͼ�ֵ
    void Clear()
    {
        mutexObject.Lock();
        keyValueMap.clear();
        mutexObject.Unlock();
    }

    //�ж�MAP���Ƿ�����ض��ļ�
    bool ContainsKey(K &key)
    {
        bool bRet = false;

        mutexObject.Lock();
        if(keyValueMap.find(key) != keyValueMap.end())
        {
            bRet = true;
        }
        mutexObject.Unlock();

        return bRet;
    }

    //�ж�MAP���Ƿ�����ض���ֵ
    bool ContainsValue(V &value)
    {
        typename std::map<K, V>::iterator iter;
        bool bRet = false;

        mutexObject.Lock();
        for(iter = keyValueMap.begin(); iter != keyValueMap.end(); ++iter)
        {
            if(iter->second == value)
            {
                bRet = true;
                break;
            }
        }
        mutexObject.Unlock();

        return bRet;
    }

    //�����ض�����ֵ,�������ڣ��򷵻�0(ע�⣺Ҫ��ȷ�ϸü��Ƿ����)
    V Get(K &key)
    {
        V value =  V();

        mutexObject.Lock();
        if(keyValueMap.find(key) != keyValueMap.end())
        {
            value = keyValueMap[key];
        }
        mutexObject.Unlock();

        return value;
    }

    //�жϴ�MAP�Ƿ�Ϊ��
    bool IsEmpty()
    {
        bool bRet = false;

        mutexObject.Lock();
        bRet = keyValueMap.empty();
        mutexObject.Unlock();

        return bRet;
    }

    //�������м���VECTOR��
    void Keys(std::vector<K> &keyVector)
    {
        typename std::map<K, V>::iterator iter;

        mutexObject.Lock();
        for(iter = keyValueMap.begin(); iter != keyValueMap.end(); ++iter)
        {
            keyVector.push_back(iter->first);
        }
        mutexObject.Unlock();
    }

    //���ָ���ļ�ֵ(KEY�����ڻ���ӣ��������滻)
    void Put(K &key, V &value)
    {
        mutexObject.Lock();
        keyValueMap[key] = value;
        mutexObject.Unlock();
    }

    //�Ƴ��ض��ļ�ֵ
    void Remove(K &key)
    {
        mutexObject.Lock();
        keyValueMap.erase(key);
        mutexObject.Unlock();
    }

    //�Ƴ��ض��ļ�ֵ(����ֵ����Ȳ��Ƴ�)
    bool Remove(K &key, V &value)
    {
        bool bRet = false;

        mutexObject.Lock();
        if(keyValueMap.find(key) != keyValueMap.end())
        {
            if(keyValueMap[key] == value)
            {
                keyValueMap.erase(key);
                bRet = true;
            }
        }
        mutexObject.Unlock();

        return bRet;
    }

    //�滻ָ������ֵ(KEY�����ڲ��滻���������򲻴���)
    void Replace(K &key, V &value)
    {
        mutexObject.Lock();
        if(keyValueMap.find(key) != keyValueMap.end())
        {
            keyValueMap[key] = value;
        }
        mutexObject.Unlock();
    }

    //�滻ָ������ֵ(KEY��VALUE�����ڲ��滻���������򲻴���)
    bool Replace(K &key, V &oldValue, V &newValue)
    {
        bool bRet = false;

        mutexObject.Lock();
        if(keyValueMap.find(key) != keyValueMap.end())
        {
            if(keyValueMap[key] == oldValue)
            {
                keyValueMap[key] = newValue;
                bRet = true;
            }
        }
        mutexObject.Unlock();

        return bRet;
    }

    //����MAP�Ĵ�С
    uint32   Size()
    {
        uint32 nCount = 0;

        mutexObject.Lock();
        nCount = keyValueMap.size();
        mutexObject.Unlock();

        return nCount;
    }

    //��������ֵ��VECTOR��
    void Values(std::vector<V> &valueVector)
    {
        typename std::map<K, V>::iterator iter;

        mutexObject.Lock();
        for(iter = keyValueMap.begin(); iter != keyValueMap.end(); ++iter)
        {
            valueVector.push_back(iter->second);
        }
        mutexObject.Unlock();
    }

    //��MAP���Ƴ���ֵ��(��ֵ���ʱ�Ƴ�)
    void RemoveValue(V &value)
    {
        typename std::map<K, V>::iterator iter;

        mutexObject.Lock();
        for(iter = keyValueMap.begin(); iter != keyValueMap.end(); ++iter)
        {
            if(iter->second == value)
            {
                keyValueMap.erase(iter);
                break;
            }
        }
        mutexObject.Unlock();
    }


private:
    std::map<K, V> keyValueMap;
    CMutex mutexObject;

private:
    DISALLOW_COPY_AND_ASSIGN(CConcurrentMap);
};


/*********************************
�߳���������(����ʵ��)
**********************************/
template <typename V>  class CLinkedBlockingQueue
{
public:

    CLinkedBlockingQueue() {}
    virtual ~CLinkedBlockingQueue()
    {
        if(!valueList.empty())
        {
            valueList.clear();
        }
    }

    //�Ӷ��г����Ƴ�����Ԫ��
    void Clear()
    {
        mutexObject.Lock();
        valueList.clear();
        mutexObject.Unlock();
    }

    //�Ӷ������Ƴ�����Ԫ�أ�����Ԫ�ش�ŵ�Vector��
    int DrainTo(std::vector<V> &valueVector)
    {
        int nCount = 0;

        mutexObject.Lock();
        nCount = valueList.size();
        if(nCount > 0)
        {
            copy(valueList.begin(),valueList.end(),back_inserter(valueVector));
            valueList.clear();
        }
        mutexObject.Unlock();

        return nCount;
    }

    //���Ӷ������Ƴ�ͷ������Ԫ�أ�����Ԫ�ش�ŵ�Vector��
    int DrainTo(std::vector<V> &valueVector,int maxElements)
    {
        typename std::list<V>::iterator iter;
        int nCount = 0;

        mutexObject.Lock();
        if(nCount > 0)
        {
            nCount = (nCount < maxElements) ? nCount : maxElements;
            iter = valueList.begin();
            advance(iter, nCount);
            copy(valueList.begin(),iter,back_inserter(valueVector));
            valueList.erase(valueList.begin(),iter);
        }
        mutexObject.Unlock();

        return nCount;
    }

    //��ָ��Ԫ�ز��뵽�˶��е�β��
    void Offer(V &value)
    {
        mutexObject.Lock();
        valueList.push_back(value);
        mutexObject.Unlock();
    }

    //��ȡ�����Ƴ��˶��е�ͷ
    V  Peek()
    {
        V tempValue = V();

        mutexObject.Lock();
        if(!valueList.empty())
        {
            tempValue = valueList.front();
        }
        mutexObject.Unlock();

        return tempValue;
    }

    //��ȡ���Ƴ��˶��е�ͷ
    V  Poll()
    {
        V tempValue = V();

        mutexObject.Lock();
        if(!valueList.empty())
        {
            tempValue = valueList.front();
            valueList.pop_front();
        }
        mutexObject.Unlock();

        return tempValue;
    }

    //��ָ��Ԫ�ز��뵽�˶��е�β��(ע�⣺�ᷢ����������ź�)
    void Put(V &value)
    {
        mutexObject.Lock();
        valueList.push_back(value);
        mutexObject.Unlock();
        semaphoreObject.Post();
    }

    //�Ӵ˶����Ƴ�ָ��Ԫ�صĵ���ʵ��
    bool  Remove(V &value)
    {
        typename std::list<V>::iterator iter;
        bool bRet = false;

        mutexObject.Lock();
        for (iter = valueList.begin(); iter != valueList.end();)
        {
            if((*iter) == value)
            {
                iter = valueList.erase(iter);
                bRet = true;
            }
            else
            {
                ++iter;
            }
        }
        mutexObject.Unlock();

        return bRet;
    }

    //���ض����е�Ԫ�ظ���
    uint32 Size()
    {
        uint32 itemCount = 0;

        mutexObject.Lock();
        itemCount = valueList.size();
        mutexObject.Unlock();

        return itemCount;
    }

    //��ȡ���Ƴ��˶��е�ͷ��(ע��:����Ԫ��ʱ֮ǰһֱ�ȴ�)
    V Take()
    {
        V tempValue = V();

        semaphoreObject.Wait();
        mutexObject.Lock();
        if(!valueList.empty())
        {
            tempValue = valueList.front();
            valueList.pop_front();
        }
        mutexObject.Unlock();

        return tempValue;
    }

    //���ذ��ʵ�˳������˶���������Ԫ�ص�����
    void ToArray(std::vector<V> &valueVector)
    {
        mutexObject.Lock();
        copy(valueList.begin(),valueList.end(),back_inserter(valueVector));
        mutexObject.Unlock();
    }

private:
    CMutex mutexObject;
    CSemaphore_1 semaphoreObject;
    std::list<V> valueList;

private:
    DISALLOW_COPY_AND_ASSIGN(CLinkedBlockingQueue);
};


template <typename V>  class CArrayBlockingQueue
{
public:

    CArrayBlockingQueue() {}
    virtual ~CArrayBlockingQueue()
    {
        if(!valueDeque.empty())
        {
            valueDeque.clear();
        }
    }

    //�Ӷ��г����Ƴ�����Ԫ��
    void Clear()
    {
        mutexObject.Lock();
        valueDeque.clear();
        mutexObject.Unlock();
    }

    //�Ӷ������Ƴ�����Ԫ�أ�����Ԫ�ش�ŵ�Vector��
    int DrainTo(std::vector<V> &valueVector)
    {
        int nCount = 0;

        mutexObject.Lock();
        nCount = valueDeque.size();
        if(nCount > 0)
        {
            copy(valueDeque.begin(),valueDeque.end(),back_inserter(valueVector));
            valueDeque.erase(valueDeque.begin(),valueDeque.end());
        }
        mutexObject.Unlock();

        return nCount;
    }

    //���Ӷ������Ƴ�ͷ������Ԫ�أ�����Ԫ�ش�ŵ�Vector��
    int DrainTo(std::vector<V> &valueVector,int maxElements)
    {
        int nCount = 0;

        mutexObject.Lock();
        nCount = valueDeque.size();
        if(nCount > 0)
        {
            nCount = (nCount < maxElements) ? nCount : maxElements;
            copy(valueDeque.begin(),valueDeque.begin() + nCount,back_inserter(valueVector));
            valueDeque.erase(valueDeque.begin(),valueDeque.begin() + nCount);
        }
        mutexObject.Unlock();

        return nCount;
    }

    //��ָ��Ԫ�ز��뵽�˶��е�β��
    void Offer(V &value)
    {
        mutexObject.Lock();
        valueDeque.push_back(value);
        mutexObject.Unlock();
    }

    //��ȡ�����Ƴ��˶��е�ͷ
    V  Peek()
    {
        V tempValue = V();

        mutexObject.Lock();
        if(!valueDeque.empty())
        {
            tempValue = valueDeque.front();
        }
        mutexObject.Unlock();

        return tempValue;
    }

    //��ȡ���Ƴ��˶��е�ͷ
    V  Poll()
    {
        V tempValue = V();

        mutexObject.Lock();
        if(!valueDeque.empty())
        {
            tempValue = valueDeque.front();
            valueDeque.pop_front();
        }
        mutexObject.Unlock();

        return tempValue;
    }

    //��ָ��Ԫ�ز��뵽�˶��е�β��(ע�⣺�˷����ᷢ����������ź�)
    void Put(V &value)
    {
        mutexObject.Lock();
        valueDeque.push_back(value);
        mutexObject.Unlock();
        semaphoreObject.Post();
    }

    //�Ӵ˶����Ƴ�ָ��Ԫ�صĵ���ʵ��
    bool  Remove(V &value)
    {
        typename std::deque<V>::iterator iter;
        bool bRet = false;

        mutexObject.Lock();
        for (iter = valueDeque.begin(); iter != valueDeque.end();)
        {
            if((*iter) == value)
            {
                iter = valueDeque.erase(iter);
                bRet = true;
            }
            else
            {
                ++iter;
            }
        }
        mutexObject.Unlock();

        return bRet;
    }

    //���ض����е�Ԫ�ظ���
    uint32 Size()
    {
        uint32 itemCount = 0;

        mutexObject.Lock();
        itemCount = valueDeque.size();
        mutexObject.Unlock();

        return itemCount;
    }

    //��ȡ���Ƴ��˶��е�ͷ��(ע�⣺��Ԫ�ر�ÿ���֮ǰһֱ�ȴ�)
    V Take()
    {
        V tempValue = V();

        semaphoreObject.Wait();
        mutexObject.Lock();
        if(!valueDeque.empty())
        {
            tempValue = valueDeque.front();
            valueDeque.pop_front();
        }
        mutexObject.Unlock();
        return tempValue;
    }

    //���ذ��ʵ�˳������˶���������Ԫ�ص�����
    void ToArray(std::vector<V> &valueVector)
    {
        mutexObject.Lock();
        copy(valueDeque.begin(),valueDeque.end(),back_inserter(valueVector));
        mutexObject.Unlock();
    }

private:
    CMutex mutexObject;
    CSemaphore_1 semaphoreObject;
    std::deque<V> valueDeque;

private:
    DISALLOW_COPY_AND_ASSIGN(CArrayBlockingQueue);
};

}



#endif /* CONTAINER_H_ */
