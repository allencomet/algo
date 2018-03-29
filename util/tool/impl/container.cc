//#include "stdafx.h"
#include "../container.h"

namespace toolHelp
{

const int32 CPtrArrayList::LAST_INDEX = -1;

CPtrArrayList::CPtrArrayList(int32 initCount)
{
    mInitCount = initCount;
    Init();
}

CPtrArrayList::~CPtrArrayList()
{
    free(mFirst);
    mFirst = NULL;
}

void  CPtrArrayList::Init()
{
    mMaxCount = mInitCount <= 0 ? 2 : mInitCount;
    mCount = 0;
    mFirst = (void**) malloc(sizeof(void *) * mMaxCount);
    memset(mFirst, 0, mMaxCount * sizeof(void*));
}

void  CPtrArrayList::Clear()
{
    if (NULL != mFirst)
    {
        free(mFirst);
    }

    Init();
}

int32  CPtrArrayList::GetCount() const
{
    return mCount;
}

bool  CPtrArrayList::Append(void * value)
{
    if (NULL == value)
        return false;

    if (mCount >= mMaxCount)
    {
        mMaxCount = (mMaxCount * 3) / 2 + 1;
        mFirst = (void**) realloc(mFirst, sizeof(void *) * mMaxCount);
        assert(NULL != mFirst);
        memset(mFirst + mCount, 0, (mMaxCount - mCount) * sizeof(void *));
    }

    mFirst[mCount++] = value;

    return true;
}

void*  CPtrArrayList::TakeItem(int32 index)
{
    void * ret = NULL;

    if (LAST_INDEX == index)
        index = mCount - 1;
    if (index < 0 || index >= mCount)
        return ret;

    ret = mFirst[index];

    mCount--;

    if ((index + 1) < mMaxCount)
    {
        memmove(mFirst + index, mFirst + index + 1, (mMaxCount - index - 1) * sizeof(void *));
    }
    else
    {
        mFirst[index] = NULL;
    }

    return ret;
}

const void*  CPtrArrayList::GetItem(int32 index) const
{
    const void * ret = NULL;

    if (LAST_INDEX == index)
    {
        index = mCount - 1;
    }

    if (index < 0 || index >= mCount)
        return ret;

    ret = mFirst[index];

    return ret;
}
/*******************************************************************************************/

CPtrCircleQueue::CPtrCircleQueue()
{
    mMaxCount = 8;
    mEntries = (void**) malloc(sizeof(void*) * mMaxCount);

    mHead = mTail = mCount = 0;
}

CPtrCircleQueue::~CPtrCircleQueue()
{
    free(mEntries);
    mEntries = NULL;
}

void  CPtrCircleQueue::Push(void* item)
{
    if (mCount >= mMaxCount)
    {
        mMaxCount = (mMaxCount * 3) / 2 + 1;
        void ** newEntries = (void**) malloc(sizeof(void *) * mMaxCount);

        unsigned int headLen = 0, tailLen = 0;
        if (mHead < mTail)
        {
            headLen = mTail - mHead;
        }
        else
        {
            headLen = mCount - mTail;
            tailLen = mTail;
        }

        memcpy(newEntries, &(mEntries[mHead]), sizeof(void *) * headLen);
        if (tailLen)
        {
            memcpy(&(newEntries[headLen]), mEntries, sizeof(void *) * tailLen);
        }

        mHead = 0;
        mTail = headLen + tailLen;

        free(mEntries);
        mEntries = newEntries;
    }

    mEntries[mTail++] = item;
    mTail = mTail % mMaxCount;
    mCount++;
}

void*  CPtrCircleQueue::Pop()
{
    void * ret = NULL;

    if (mCount > 0)
    {
        ret = mEntries[mHead++];
        mHead = mHead % mMaxCount;
        mCount--;
    }

    return ret;
}

void*  CPtrCircleQueue::Front()
{
	void * ret = NULL;

	if (mCount > 0)
	{
		ret = mEntries[mHead];
	}

	return ret;
}

void*  CPtrCircleQueue::Top()
{
    return mCount > 0 ? mEntries[mHead] : NULL;
}

int  CPtrCircleQueue::GetLength()
{
    return mCount;
}


/*******************************************************************************************/
CBlockingQueue::CBlockingQueue()
{
    mQueue = new CPtrCircleQueue();
    pthread_mutex_init(&mMutex, NULL);
    pthread_cond_init(&mCond, NULL);
}

CBlockingQueue::~CBlockingQueue()
{
    delete mQueue;
    pthread_mutex_destroy(&mMutex);
    pthread_cond_destroy(&mCond);
}

void  CBlockingQueue::Push(void * item)
{
    pthread_mutex_lock(&mMutex);

    mQueue->Push(item);

    pthread_cond_signal(&mCond);

    pthread_mutex_unlock(&mMutex);
}

void*  CBlockingQueue::Pop()
{
    void * ret = NULL;

    pthread_mutex_lock(&mMutex);

    if(mQueue->GetLength() == 0)
    {
        pthread_cond_wait(&mCond, &mMutex);
    }

    ret = mQueue->Pop();

    pthread_mutex_unlock(&mMutex);

    return ret;
}

//阻塞，若没有数据，则等待，有数据才返回,并且不会删除队列中元素
void* CBlockingQueue::Front() {
	void * ret = NULL;

	pthread_mutex_lock(&mMutex);

	if (mQueue->GetLength() == 0)
	{
		pthread_cond_wait(&mCond, &mMutex);
	}

	ret = mQueue->Front();

	pthread_mutex_unlock(&mMutex);

	return ret;
}

void*  CBlockingQueue::Top()
{
    void * ret = NULL;

    pthread_mutex_lock(&mMutex);

    ret = mQueue->Top();

    pthread_mutex_unlock(&mMutex);

    return ret;
}

int  CBlockingQueue::GetLength()
{
    int len = 0;

    pthread_mutex_lock(&mMutex);

    len = mQueue->GetLength();

    pthread_mutex_unlock(&mMutex);

    return len;
}


}
