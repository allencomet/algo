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

    //非阻塞，添加数据进队列
    void Push(void *item);

    //阻塞，若没有数据，则等待，有数据才返回
    void* Pop();

	//阻塞，若没有数据，则等待，有数据才返回,并且不会删除队列中元素
	void* Front();

    //非阻塞，若没有数据，则直接返回NULL
    void* Top();

    //非阻塞，返回队列长度
    int GetLength();

private:
    CPtrCircleQueue * mQueue;
    pthread_mutex_t mMutex;
    pthread_cond_t mCond;
};



/*********************************
数组列表对象
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

    //将指定的元素添加到此列表的尾部
    void  Add(V &value)
    {
        valueVector.push_back(value);
    }

    //将指定的元素插入此列表中的指定位置(注意:index必须0<=index<Size)
    void Add(int index, V &value)
    {
        if(index >=0 && index < valueVector.size())
        {
            valueVector.insert(valueVector.begin() + index,value);
        }
    }

    //移除此列表中的所有元素
    void Clear()
    {
        valueVector.clear();
    }

    //判断列表中是否包含某元素
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

    //返回此列表中指定位置上的元素(注意:index必须0<=index<Size);
    V  Get(int index)
    {
        V tempValue = V();

        if(index >=0 && index < valueVector.size())
        {
            tempValue = valueVector[index];
        }

        return tempValue;
    }

    //返回此列表中首次出现的指定元素的索引，或如果此列表不包含元素，则返回-1
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

    //判断列表是否为空
    bool IsEmpty()
    {
        bool bRet = false;
        bRet = valueVector.empty();
        return bRet;
    }


    //返回此列表中最后一次出现的指定元素的索引，或如果此列表不包含索引，则返回-1。
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

    //移除此列表中指定位置上的元素(注意:index必须0<=index<Size)
    void Remove(int index)
    {
        if(index >=0 && index < valueVector.size())
        {
            valueVector.erase(valueVector.begin() + index);
        }
    }

    //移除此列表中首次出现的指定元素(如果存在)
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

    //移除此列表中出现的指定元素(如果存在多个，则全部移除)
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

    //用指定的元素替代此列表中指定位置上的元素(注意:index必须0<=index<Size)
    void Set(int index, V &value)
    {
        if(index >=0 && index < valueVector.size())
        {
            valueVector[index] = value;
        }
    }

    //返回此列表中的元素数
    uint32 Size()
    {
        return valueVector.size();
    }

    //按顺序返回包含此列表中所有元素的vector
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
线程并发数组
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

    //将指定的元素添加到此列表的尾部
    void  Add(V &value)
    {
        mutexObject.Lock();
        valueVector.push_back(value);
        mutexObject.Unlock();
    }

    //将指定的元素插入此列表中的指定位置(注意:index必须0<=index<Size)
    void Add(int index, V &value)
    {
        mutexObject.Lock();
        if(index >=0 && index < valueVector.size())
        {
            valueVector.insert(valueVector.begin() + index,value);
        }
        mutexObject.Unlock();
    }

    //移除此列表中的所有元素
    void Clear()
    {
        mutexObject.Lock();
        valueVector.clear();
        mutexObject.Unlock();
    }

    //判断列表中是否包含某元素
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

    //返回此列表中指定位置上的元素(注意:index必须0<=index<Size)
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

    //返回此列表中首次出现的指定元素的索引，或如果此列表不包含元素，则返回-1
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

    //判断列表是否为空
    bool IsEmpty()
    {
        bool bRet = false;

        mutexObject.Lock();
        bRet = valueVector.empty();
        mutexObject.Unlock();

        return bRet;
    }


    //返回此列表中最后一次出现的指定元素的索引，或如果此列表不包含索引，则返回-1。
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

    //移除此列表中指定位置上的元素(注意:index必须0<=index<Size)
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

    //移除此列表中首次出现的指定元素(如果存在)
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

    //移除此列表中出现的指定元素(如果存在多个，则全部移除)
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


    //用指定的元素替代此列表中指定位置上的元素(注意:index必须0<=index<Size)
    void Set(int index, V &value)
    {
        mutexObject.Lock();
        if(index >=0 && index < valueVector.size())
        {
            valueVector[index] = value;
        }
        mutexObject.Unlock();
    }

    //返回此列表中的元素数
    uint32 Size()
    {
        uint32 itemCount = 0;

        mutexObject.Lock();
        itemCount = valueVector.size();
        mutexObject.Unlock();

        return itemCount;
    }

    //按顺序返回包含此列表中所有元素的vector
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
数组对列
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

    //将指定元素插入此双端队列的末尾
    void  Add(V &value)
    {
        valueQueue.push_back(value);
    }

    //将指定元素插入此双端队列的开头
    void AddFirst(V &value)
    {
        valueQueue.push_front(value);
    }

    //将指定元素插入此双端队列的末尾
    void AddLast(V &value)
    {
        valueQueue.push_back(value);
    }

    //从此双端队列中移除所有元素
    void Clear()
    {
        valueQueue.clear();
    }

    //判断此对列上是否包含特定元素
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

    //获取但不移除此双端队列所表示的队列的头(注意:对列必须非空)
    V  Element()
    {
        V tempValue = V();
        if(!valueQueue.empty())
        {
            tempValue = valueQueue.front();
        }
        return tempValue;
    }

    //获取但不移除此双端队列的第一个元素(注意:对列必须非空)
    V  GetFirst()
    {
        V tempValue = V();
        if(!valueQueue.empty())
        {
            tempValue = valueQueue.front();
        }
        return tempValue;
    }

    //获取但不移除此双端队列的最后一个元素(注意:对列必须非空)
    V  GetLast()
    {
        V tempValue = V();
        if(!valueQueue.empty())
        {
            tempValue = valueQueue.back();
        }
        return tempValue;
    }

    //判断此对列是否为空
    bool IsEmpty()
    {
        bool bRet = false;
        bRet = valueQueue.empty();
        return bRet;
    }

    //将指定元素插入此双端队列的末尾
    void Offer(V &value)
    {
        valueQueue.push_back(value);
    }

    //将指定元素插入此双端队列的开头
    void OfferFisrt(V &value)
    {
        valueQueue.push_front(value);
    }

    //将指定元素插入此双端队列的开头
    void OfferLast(V &value)
    {
        valueQueue.push_back(value);
    }

    //获取但不移除此双端队列所表示的队列的头(注意:对列必须非空)
    V  Peek()
    {
        V tempValue = V();
        if(!valueQueue.empty())
        {
            tempValue = valueQueue.front();
        }
        return tempValue;
    }

    //获取但不移除此双端队列的第一个元素(注意:对列必须非空)
    V  PeekFirst()
    {
        V tempValue = V();

        if(!valueQueue.empty())
        {
            tempValue = valueQueue.front();
        }

        return tempValue;
    }


    //获取但不移除此双端队列的最后一个元素(注意:对列必须非空)
    V  PeekLast()
    {
        V tempValue = V();

        if(!valueQueue.empty())
        {
            tempValue = valueQueue.back();
        }

        return tempValue;
    }

    //获取并移除此双端队列所表示的队列的头(注意:对列必须非空)
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

    //获取并移除此双端队列所的第一个元素(注意:对列必须非空)
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

    //获取并移除此双端队列所的最后一个元素(注意:对列必须非空)
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

    //从此双端队列所表示的堆栈中弹出一个元素(注意:对列必须非空)
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

    //将元素推入此双端队列所表示的堆栈
    void Push(V &value)
    {
        valueQueue.push_front(value);
    }

    //获取并移除此双端队列所表示的队列的头(注意:对列必须非空)
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

    //获取并移除此双端队列所的第一个元素(注意:对列必须非空)
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

    //获取并移除此双端队列所的最后一个元素(注意:对列必须非空)
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

    //从此双端队列中移除指定元素(多个全部移除)
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


    //移除此双端队列中第一次出现的指定元素(当从头部到尾部遍历双端队列时)
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

    //移除此双端队列中最后一次出现的指定元素(当从头部到尾部遍历双端队列时)
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

    //返回此双端队列中的元素数
    uint32 Size()
    {
        return valueQueue.size();
    }

    //按顺序返回包含此列表中所有元素的vector
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
线程并发数组对列
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

    //将指定元素插入此双端队列的末尾
    void  Add(V &value)
    {
        mutexObject.Lock();
        valueQueue.push_back(value);
        mutexObject.Unlock();
    }

    //将指定元素插入此双端队列的开头
    void AddFirst(V &value)
    {
        mutexObject.Lock();
        valueQueue.push_front(value);
        mutexObject.Unlock();
    }

    //将指定元素插入此双端队列的末尾
    void AddLast(V &value)
    {
        mutexObject.Lock();
        valueQueue.push_back(value);
        mutexObject.Unlock();
    }

    //从此双端队列中移除所有元素
    void Clear()
    {
        mutexObject.Lock();
        valueQueue.clear();
        mutexObject.Unlock();
    }

    //判断此对列上是否包含特定元素
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

    //获取但不移除此双端队列所表示的队列的头(注意:对列必须非空)
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

    //获取但不移除此双端队列的第一个元素(注意:对列必须非空)
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

    //获取但不移除此双端队列的最后一个元素(注意:对列必须非空)
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

    //判断此对列是否为空
    bool IsEmpty()
    {
        bool bRet = false;

        mutexObject.Lock();
        bRet = valueQueue.empty();
        mutexObject.Unlock();

        return bRet;
    }

    //将指定元素插入此双端队列的末尾
    void Offer(V &value)
    {
        mutexObject.Lock();
        valueQueue.push_back(value);
        mutexObject.Unlock();
    }

    //将指定元素插入此双端队列的开头
    void OfferFisrt(V &value)
    {
        mutexObject.Lock();
        valueQueue.push_front(value);
        mutexObject.Unlock();
    }

    //将指定元素插入此双端队列的开头
    void OfferLast(V &value)
    {
        mutexObject.Lock();
        valueQueue.push_back(value);
        mutexObject.Unlock();
    }

    //获取但不移除此双端队列所表示的队列的头(注意:对列必须非空)
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

    //获取但不移除此双端队列的第一个元素(注意:对列必须非空)
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


    //获取但不移除此双端队列的最后一个元素(注意:对列必须非空)
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

    //获取并移除此双端队列所表示的队列的头(注意:对列必须非空)
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

    //获取并移除此双端队列所的第一个元素(注意:对列必须非空)
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

    //获取并移除此双端队列所的最后一个元素(注意:对列必须非空)
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

    //从此双端队列所表示的堆栈中弹出一个元素(注意:对列必须非空)
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

    //将元素推入此双端队列所表示的堆栈
    void Push(V &value)
    {
        mutexObject.Lock();
        valueQueue.push_front(value);
        mutexObject.Unlock();
    }

    //获取并移除此双端队列所表示的队列的头(注意:对列必须非空)
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

    //获取并移除此双端队列所的第一个元素(注意:对列必须非空)
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

    //获取并移除此双端队列所的最后一个元素(注意:对列必须非空)
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

    //从此双端队列中移除指定元素(多个全部移除)
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


    //移除此双端队列中第一次出现的指定元素(当从头部到尾部遍历双端队列时)
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

    //移除此双端队列中最后一次出现的指定元素(当从头部到尾部遍历双端队列时)
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

    //返回此双端队列中的元素数
    uint32 Size()
    {
        uint32 itemCount = 0;

        mutexObject.Lock();
        itemCount = valueQueue.size();
        mutexObject.Unlock();

        return itemCount;
    }

    //按顺序返回包含此列表中所有元素的vector
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
链接对列
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

    //将指定元素插入此链表的末尾
    void  Add(V &value)
    {
        valueList.push_back(value);
    }

    //将指定元素插入此链表的指定位置(注意：index必须 0<=index<size);
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

    //将指定元素插入此链表的开头
    void AddFirst(V &value)
    {
        valueList.push_front(value);
    }

    //将指定元素插入此链表的末尾
    void AddLast(V &value)
    {
        valueList.push_back(value);
    }

    //从此链表中移除所有元素
    void Clear()
    {
        valueList.clear();
    }

    //判断此链表上是否包含特定元素
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

    //获取但不移除此链表的头(注意:链表必须非空)
    V  Element()
    {
        V tempValue = V();

        if(!valueList.empty())
        {
            tempValue = valueList.front();
        }

        return tempValue;
    }

    //返回此链表中指定位置处的元素(注意：index必须0<=index<size)
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

    //获取但不移除此链表的第一个元素(注意:链表必须非空)
    V  GetFirst()
    {
        V tempValue = V();

        if(!valueList.empty())
        {
            tempValue = valueList.front();
        }

        return tempValue;
    }

    //获取但不移除此链表的最后一个元素(注意:对列必须非空)
    V  GetLast()
    {
        V tempValue = V();

        if(!valueList.empty())
        {
            tempValue = valueList.back();
        }

        return tempValue;
    }

    //返回此链表中首次出现的指定元素的索引，或如果此列表不包含元素，则返回-1
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

    //返回此链表中最后出现的指定元素的索引，或如果此列表不包含元素，则返回-1
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

    //判断此对列是否为空
    bool IsEmpty()
    {
        bool bRet = false;
        bRet = valueList.empty();
        return bRet;
    }

    //将指定元素插入此链表的末尾
    void Offer(V &value)
    {
        valueList.push_back(value);
    }

    //将指定元素插入此链表的开头
    void OfferFisrt(V &value)
    {
        valueList.push_front(value);
    }

    //将指定元素插入此链表的开头
    void OfferLast(V &value)
    {
        valueList.push_back(value);
    }

    //获取但不移除此链表的头(注意:链表必须非空)
    V  Peek()
    {
        V tempValue = V();

        if(!valueList.empty())
        {
            tempValue = valueList.front();
        }

        return tempValue;
    }

    //获取但不移除此链表的第一个元素(注意:链表必须非空)
    V  PeekFirst()
    {
        V tempValue = V();

        if(!valueList.empty())
        {
            tempValue = valueList.front();
        }

        return tempValue;
    }


    //获取但不移除此链表的最后一个元素(注意:链表必须非空)
    V  PeekLast()
    {
        V tempValue = V();

        if(!valueList.empty())
        {
            tempValue = valueList.back();
        }

        return tempValue;
    }

    //获取并移除此链表的头(注意:链表必须非空)
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

    //获取并移除此链表所的第一个元素(注意:链表必须非空)
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

    //获取并移除此链表所的最后一个元素(注意:链表必须非空)
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

    //从此链表所表示的堆栈中弹出一个元素(注意:链表必须非空)
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

    //将元素推入此链表所表示的堆栈
    void Push(V &value)
    {
        valueList.push_front(value);
    }

    //获取并移除此链表所表示的队列的头(注意:链表必须非空)
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

    //获取并移除此链表所的第一个元素(注意:链表必须非空)
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

    //获取并移除此链表所的最后一个元素(注意:链表必须非空)
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

    //从此链表中移除指定元素(多个全部移除)
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


    //移除此链表中第一次出现的指定元素(当从头部到尾部遍历链表时)
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

    //移除此链表中最后一次出现的指定元素(当从头部到尾部遍历链表时)
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

    //将此列表中指定位置的元素替换为指定的元素，返回以前的元素(注意：index必须 0<=index<size);
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


    //返回此链表中的元素数
    uint32 Size()
    {
        return valueList.size();
    }

    //按顺序返回包含此列表中所有元素的vector
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
线程并发链接对列
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

    //将指定元素插入此链表的末尾
    void  Add(V &value)
    {
        mutexObject.Lock();
        valueList.push_back(value);
        mutexObject.Unlock();
    }

    //将指定元素插入此链表的指定位置(注意：index必须 0<=index<size);
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

    //将指定元素插入此链表的开头
    void AddFirst(V &value)
    {
        mutexObject.Lock();
        valueList.push_front(value);
        mutexObject.Unlock();
    }

    //将指定元素插入此链表的末尾
    void AddLast(V &value)
    {
        mutexObject.Lock();
        valueList.push_back(value);
        mutexObject.Unlock();
    }

    //从此链表中移除所有元素
    void Clear()
    {
        mutexObject.Lock();
        valueList.clear();
        mutexObject.Unlock();
    }

    //判断此链表上是否包含特定元素
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

    //获取但不移除此链表的头(注意:链表必须非空)
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

    //返回此链表中指定位置处的元素(注意：index必须0<=index<size)
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

    //获取但不移除此链表的第一个元素(注意:链表必须非空)
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

    //获取但不移除此链表的最后一个元素(注意:对列必须非空)
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

    //返回此链表中首次出现的指定元素的索引，或如果此列表不包含元素，则返回-1
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

    //返回此链表中最后出现的指定元素的索引，或如果此列表不包含元素，则返回-1
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

    //判断此对列是否为空
    bool IsEmpty()
    {
        bool bRet = false;

        mutexObject.Lock();
        bRet = valueList.empty();
        mutexObject.Unlock();

        return bRet;
    }

    //将指定元素插入此链表的末尾
    void Offer(V &value)
    {
        mutexObject.Lock();
        valueList.push_back(value);
        mutexObject.Unlock();
    }

    //将指定元素插入此链表的开头
    void OfferFisrt(V &value)
    {
        mutexObject.Lock();
        valueList.push_front(value);
        mutexObject.Unlock();
    }

    //将指定元素插入此链表的开头
    void OfferLast(V &value)
    {
        mutexObject.Lock();
        valueList.push_back(value);
        mutexObject.Unlock();
    }

    //获取但不移除此链表的头(注意:链表必须非空)
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

    //获取但不移除此链表的第一个元素(注意:链表必须非空)
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


    //获取但不移除此链表的最后一个元素(注意:链表必须非空)
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

    //获取并移除此链表的头(注意:链表必须非空)
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

    //获取并移除此链表所的第一个元素(注意:链表必须非空)
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

    //获取并移除此链表所的最后一个元素(注意:链表必须非空)
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

    //从此链表所表示的堆栈中弹出一个元素(注意:链表必须非空)
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

    //将元素推入此链表所表示的堆栈
    void Push(V &value)
    {
        mutexObject.Lock();
        valueList.push_front(value);
        mutexObject.Unlock();
    }

    //获取并移除此链表所表示的队列的头(注意:链表必须非空)
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

    //获取并移除此链表所的第一个元素(注意:链表必须非空)
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

    //获取并移除此链表所的最后一个元素(注意:链表必须非空)
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

    //从此链表中移除指定元素(多个全部移除)
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


    //移除此链表中第一次出现的指定元素(当从头部到尾部遍历链表时)
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

    //移除此链表中最后一次出现的指定元素(当从头部到尾部遍历链表时)
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

    //将此列表中指定位置的元素替换为指定的元素，返回原来的元素(注意：index必须 0<=index<size);
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


    //返回此链表中的元素数
    uint32 Size()
    {
        uint32 itemCount = 0;

        mutexObject.Lock();
        itemCount = valueList.size();
        mutexObject.Unlock();

        return itemCount;
    }

    //按顺序返回包含此列表中所有元素的vector
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
MAP对象(KEY使用HASH存储)
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

    //删除所有键和键值
    void Clear()
    {
        keyValueHashMap.clear();
    }

    //判断MAP中是否存在特定的键
    bool ContainsKey(K &key)
    {
        bool bRet = false;

        if(keyValueHashMap.find(key) != keyValueHashMap.end())
        {
            bRet = true;
        }

        return bRet;
    }

    //判断MAP中是否存在特定的值
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

    //返回特定键的值(注意：要先确认该键是否存在)
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

    //判断此MAP是否为空
    bool IsEmpty()
    {
        return  keyValueHashMap.empty();
    }

    //返回所有键到VECTOR中
    void Keys(std::vector<K> &keyVector)
    {
        typename boost::unordered_map<K, V>::iterator iter;

        for(iter = keyValueHashMap.begin(); iter != keyValueHashMap.end(); ++iter)
        {
            keyVector.push_back(iter->first);
        }
    }

    //添加指定的键值(KEY不存在会添加，存在仍替换)
    void Put(K &key, V &value)
    {
        keyValueHashMap[key] = value;
    }

    //移除特定的键值
    void Remove(K &key)
    {
        keyValueHashMap.erase(key);
    }

    //移除特定的键值(键和值都相等才移除)
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

    //替换指定键的值(KEY若存在才替换，不存在则不处理)
    void Replace(K &key, V &value)
    {
        if(keyValueHashMap.find(key) != keyValueHashMap.end())
        {
            keyValueHashMap[key] = value;
        }
    }

    //替换指定键的值(KEY和VALUE都存在才替换，不存在则不处理)
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

    //返回MAP的大小
    uint32   Size()
    {
        return keyValueHashMap.size();
    }

    //返回所有值到VECTOR中
    void Values(std::vector<V> &valueVector)
    {
        typename boost::unordered_map<K, V>::iterator iter;

        for(iter = keyValueHashMap.begin(); iter != keyValueHashMap.end(); ++iter)
        {
            valueVector.push_back(iter->second);
        }
    }

    //从MAP中移出键值对(当值相等时移除)
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
线程同步HASH_MAP(KEY使用HASH存储)
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

    //删除所有键和键值
    void Clear()
    {
        mutexObject.Lock();
        keyValueHashMap.clear();
        mutexObject.Unlock();
    }

    //判断MAP中是否存在特定的键
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

    //判断MAP中是否存在特定的值
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

    //返回特定键的值,若不存在，则返回0(注意：要先确认该键是否存在)
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

    //判断此MAP是否为空
    bool IsEmpty()
    {
        bool bRet = false;

        mutexObject.Lock();
        bRet = keyValueHashMap.empty();
        mutexObject.Unlock();

        return bRet;
    }

    //返回所有键到VECTOR中
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

    //添加指定的键值(KEY不存在会添加，存在仍替换)
    void Put(K &key, V &value)
    {
        mutexObject.Lock();
        keyValueHashMap[key] = value;
        mutexObject.Unlock();
    }

    //移除特定的键值
    void Remove(K &key)
    {
        mutexObject.Lock();
        keyValueHashMap.erase(key);
        mutexObject.Unlock();
    }

    //移除特定的键值(键和值都相等才移除)
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

    //替换指定键的值(KEY若存在才替换，不存在则不处理)
    void Replace(K &key, V &value)
    {
        mutexObject.Lock();
        if(keyValueHashMap.find(key) != keyValueHashMap.end())
        {
            keyValueHashMap[key] = value;
        }
        mutexObject.Unlock();
    }

    //替换指定键的值(KEY和VALUE都存在才替换，不存在则不处理)
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

    //返回MAP的大小
    uint32   Size()
    {
        uint32 nCount = 0;

        mutexObject.Lock();
        nCount = keyValueHashMap.size();
        mutexObject.Unlock();

        return nCount;
    }

    //返回所有值到VECTOR中
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

    //从MAP中移出键值对(当值相等时移除)
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
MAP(KEY使用红黑树存储)
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

    //删除所有键和键值
    void Clear()
    {
        keyValueMap.clear();
    }

    //判断MAP中是否存在特定的键
    bool ContainsKey(K &key)
    {
        bool bRet = false;

        if(keyValueMap.find(key) != keyValueMap.end())
        {
            bRet = true;
        }

        return bRet;
    }

    //判断MAP中是否存在特定的值
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

    //返回特定键的值,若不存在，则返回0(注意：要先确认该键是否存在)
    V Get(K &key)
    {
        V value = V();

        if(keyValueMap.find(key) != keyValueMap.end())
        {
            value = keyValueMap[key];
        }

        return value;
    }

    //判断此MAP是否为空
    bool IsEmpty()
    {
        bool bRet = false;
        bRet = keyValueMap.empty();
        return bRet;
    }

    //返回所有键到VECTOR中
    void Keys(std::vector<K> &keyVector)
    {
        typename std::map<K, V>::iterator iter;

        for(iter = keyValueMap.begin(); iter != keyValueMap.end(); ++iter)
        {
            keyVector.push_back(iter->first);
        }
    }

    //添加指定的键值(KEY不存在会添加，存在仍替换)
    void Put(K &key, V &value)
    {
        keyValueMap[key] = value;
    }

    //移除特定的键值
    void Remove(K &key)
    {
        keyValueMap.erase(key);
    }

    //移除特定的键值(键和值都相等才移除)
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

    //替换指定键的值(KEY若存在才替换，不存在则不处理)
    void Replace(K &key, V &value)
    {
        if(keyValueMap.find(key) != keyValueMap.end())
        {
            keyValueMap[key] = value;
        }
    }

    //替换指定键的值(KEY和VALUE都存在才替换，不存在则不处理)
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

    //返回MAP的大小
    uint32   Size()
    {
        return keyValueMap.size();
    }

    //返回所有值到VECTOR中
    void Values(std::vector<V> &valueVector)
    {
        typename std::map<K, V>::iterator iter;

        for(iter = keyValueMap.begin(); iter != keyValueMap.end(); ++iter)
        {
            valueVector.push_back(iter->second);
        }
    }

    //从MAP中移出键值对(当值相等时移除)
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
线程同步MAP(KEY使用红黑树存储)
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

    //删除所有键和键值
    void Clear()
    {
        mutexObject.Lock();
        keyValueMap.clear();
        mutexObject.Unlock();
    }

    //判断MAP中是否存在特定的键
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

    //判断MAP中是否存在特定的值
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

    //返回特定键的值,若不存在，则返回0(注意：要先确认该键是否存在)
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

    //判断此MAP是否为空
    bool IsEmpty()
    {
        bool bRet = false;

        mutexObject.Lock();
        bRet = keyValueMap.empty();
        mutexObject.Unlock();

        return bRet;
    }

    //返回所有键到VECTOR中
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

    //添加指定的键值(KEY不存在会添加，存在仍替换)
    void Put(K &key, V &value)
    {
        mutexObject.Lock();
        keyValueMap[key] = value;
        mutexObject.Unlock();
    }

    //移除特定的键值
    void Remove(K &key)
    {
        mutexObject.Lock();
        keyValueMap.erase(key);
        mutexObject.Unlock();
    }

    //移除特定的键值(键和值都相等才移除)
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

    //替换指定键的值(KEY若存在才替换，不存在则不处理)
    void Replace(K &key, V &value)
    {
        mutexObject.Lock();
        if(keyValueMap.find(key) != keyValueMap.end())
        {
            keyValueMap[key] = value;
        }
        mutexObject.Unlock();
    }

    //替换指定键的值(KEY和VALUE都存在才替换，不存在则不处理)
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

    //返回MAP的大小
    uint32   Size()
    {
        uint32 nCount = 0;

        mutexObject.Lock();
        nCount = keyValueMap.size();
        mutexObject.Unlock();

        return nCount;
    }

    //返回所有值到VECTOR中
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

    //从MAP中移出键值对(当值相等时移除)
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
线程阻塞队列(链表实现)
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

    //从队列彻底移除所有元素
    void Clear()
    {
        mutexObject.Lock();
        valueList.clear();
        mutexObject.Unlock();
    }

    //从对列中移除所有元素，并把元素存放到Vector中
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

    //最多从对列中移除头部几个元素，并把元素存放到Vector中
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

    //将指定元素插入到此队列的尾部
    void Offer(V &value)
    {
        mutexObject.Lock();
        valueList.push_back(value);
        mutexObject.Unlock();
    }

    //获取但不移除此队列的头
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

    //获取并移除此队列的头
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

    //将指定元素插入到此队列的尾部(注意：会发出解出阻塞信号)
    void Put(V &value)
    {
        mutexObject.Lock();
        valueList.push_back(value);
        mutexObject.Unlock();
        semaphoreObject.Post();
    }

    //从此队列移除指定元素的单个实例
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

    //返回队列中的元素个数
    uint32 Size()
    {
        uint32 itemCount = 0;

        mutexObject.Lock();
        itemCount = valueList.size();
        mutexObject.Unlock();

        return itemCount;
    }

    //获取并移除此队列的头部(注意:在无元素时之前一直等待)
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

    //返回按适当顺序包含此队列中所有元素的数组
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

    //从队列彻底移除所有元素
    void Clear()
    {
        mutexObject.Lock();
        valueDeque.clear();
        mutexObject.Unlock();
    }

    //从对列中移除所有元素，并把元素存放到Vector中
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

    //最多从对列中移除头部几个元素，并把元素存放到Vector中
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

    //将指定元素插入到此队列的尾部
    void Offer(V &value)
    {
        mutexObject.Lock();
        valueDeque.push_back(value);
        mutexObject.Unlock();
    }

    //获取但不移除此队列的头
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

    //获取并移除此队列的头
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

    //将指定元素插入到此队列的尾部(注意：此方法会发出解除阻塞信号)
    void Put(V &value)
    {
        mutexObject.Lock();
        valueDeque.push_back(value);
        mutexObject.Unlock();
        semaphoreObject.Post();
    }

    //从此队列移除指定元素的单个实例
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

    //返回队列中的元素个数
    uint32 Size()
    {
        uint32 itemCount = 0;

        mutexObject.Lock();
        itemCount = valueDeque.size();
        mutexObject.Unlock();

        return itemCount;
    }

    //获取并移除此队列的头部(注意：在元素变得可用之前一直等待)
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

    //返回按适当顺序包含此队列中所有元素的数组
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
