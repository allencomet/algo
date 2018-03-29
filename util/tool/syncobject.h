#ifndef SYNCOBJECT_H_
#define SYNCOBJECT_H_


#include "../base/data_types.h"
#include "../base/base.h"
#include <sys/shm.h> 
#include <sys/sem.h>  
#include <pthread.h>
#include <semaphore.h>


namespace toolHelp
{

//#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
//    TypeName(const TypeName&);             \
//void operator=(const TypeName&)

/*********************************
同步对象基础类
**********************************/
class CSyncObject
{
public:
    CSyncObject() { isInitialized = false; }
    virtual ~CSyncObject() { }
    virtual int32 Lock  () = 0;
    virtual int32 Unlock() = 0;

    bool IsInitialized() { return isInitialized; }

protected:
    bool isInitialized;
};

/*********************************
线程互斥类
**********************************/
class CMutex : public CSyncObject
{
public:
    CMutex();
    virtual ~CMutex();
    pthread_mutex_t* GetMutexHandle();

    virtual int32 Lock();
    virtual int32 Unlock();
    virtual int32 TryLock();

protected:
    pthread_mutex_t      mutexHandle;
    pthread_mutexattr_t  mutexAttribute;

private:
    DISALLOW_COPY_AND_ASSIGN(CMutex);
};


/*********************************
线程读写锁
**********************************/
class CReadWriteMutex : public CSyncObject
{
public:
    CReadWriteMutex();
    virtual ~CReadWriteMutex();
    pthread_rwlock_t* GetReadWriteMutexHandle();
    virtual int32 LockRead();
    virtual int32 LockWrite();
    virtual int32 Unlock();
    virtual int32 TryLock();
    virtual int32 Lock();

protected:
    pthread_rwlock_t      readWriteMutexHandle;
    pthread_rwlockattr_t  readWriteMutexHandleAttr;

private:
    DISALLOW_COPY_AND_ASSIGN(CReadWriteMutex);
};


/*********************************
线程条件类
**********************************/
class CCondition : public CSyncObject
{
public:
    CCondition();
    virtual ~CCondition() ;
    void    Wait();
    bool    WaitTimeout(int ms);

    void    Signal() ;
    void    SignalAll() ;

protected:
    virtual int32 Lock();
    virtual int32 Unlock();

private:
    int activeWaiters;
    bool isSendSignal;
    pthread_cond_t conditionHandle;
    CMutex mutexObject;

private:
    DISALLOW_COPY_AND_ASSIGN(CCondition);
};

/*********************************
线程自旋锁
**********************************/
class CSpinLock : public CSyncObject
{
public:
    CSpinLock();
    virtual ~CSpinLock();
    pthread_spinlock_t* GetSpinLockHandle();
    virtual int32 Lock();
    virtual int32 Unlock();
    virtual int32 TryLock();

private:
    pthread_spinlock_t spinLockHandle;

private:
    DISALLOW_COPY_AND_ASSIGN(CSpinLock);
};

/*********************************
线程信号量
**********************************/
class CSemaphore_1 : public CSyncObject
{
public:
    CSemaphore_1(uint32 initialCount = 0);
    virtual ~CSemaphore_1();

public:
    virtual int32 Lock();
    virtual int32 Unlock();
    virtual int32 Post();
    virtual int32 Wait();
    int32 TryLock(uint32 milliseconds);
    int32 Peek();

private:
    sem_t semaphoreHandle;

private:
    DISALLOW_COPY_AND_ASSIGN(CSemaphore_1);
};



static inline uint32 AtomicCompareAndSet(uint32 *lock, uint32 old, uint32 set)
{
    uint8 res;
    __asm__ volatile (
        "lock;"
        "cmpxchgl %3, %1;"
        "sete    %0;"
        : "=a" (res) : "m" (*lock), "a" (old), "r" (set) : "cc", "memory");
    return res;
}

static inline int32 AtomicAdd(uint32 *value, int32 add)
{
    __asm__ volatile (
        "lock;"
        "xaddl  %0, %1;"
        :"+r" (add) : "m" (*value) : "cc", "memory");
    return add;
}

static inline int32 AtomicIncrement(uint32 *value)
{
    return AtomicAdd(value, 1);
}

static inline int32 AtomicDecrement(uint32 *value)
{
    return AtomicAdd(value, -1);
}



/*********************************
原子计数器类
**********************************/
class AtomicLong
{
public:
    AtomicLong(long initialValue = 0)
    {
        m_lValue = initialValue;
    }
    virtual ~AtomicLong(){}

public:
    long  AddAndGet(long delta)
    {
        long value = 0;

        mutexObject.Lock();
        m_lValue += delta;
        value = m_lValue;
        mutexObject.Unlock();

        return value;
    }

    long DecrementAndGet()
    {
        long value = 0;

        mutexObject.Lock();
        m_lValue -= 1;
        value = m_lValue;
        mutexObject.Unlock();

        return value;
    }

    void Decrement()
    {
        mutexObject.Lock();
        m_lValue -= 1;
        mutexObject.Unlock();
    }

    double DoubleValue()
    {
        double value = 0;

        mutexObject.Lock();
        value = (double)m_lValue;
        mutexObject.Unlock();

        return value;
    }

    float FloatValue()
    {
        float value = 0;

        mutexObject.Lock();
        value = (float)m_lValue;
        mutexObject.Unlock();

        return value;
    }

    long Get()
    {
        long value = 0;

        mutexObject.Lock();
        value = m_lValue;
        mutexObject.Unlock();

        return value;
    }

    long GetAndAdd(long delta)
    {
        long value = 0;

        mutexObject.Lock();
        value = m_lValue;
        m_lValue += delta;
        mutexObject.Unlock();

        return value;
    }

    long GetAndDecrement()
    {
        long value = 0;

        mutexObject.Lock();
        value = m_lValue;
        m_lValue -= 1;
        mutexObject.Unlock();

        return value;
    }

    long GetAndIncrement()
    {
        long value = 0;

        mutexObject.Lock();
        value = m_lValue;
        m_lValue += 1;
        mutexObject.Unlock();

        return value;
    }

    long GetAndSet(long newValue)
    {
        long value = 0;

        mutexObject.Lock();
        value = m_lValue;
        m_lValue = newValue;
        mutexObject.Unlock();

        return value;
    }

    long IncrementAndGet()
    {
        long value = 0;

        mutexObject.Lock();
        m_lValue += 1;
        value = m_lValue;
        mutexObject.Unlock();

        return value;
    }

    void Increment()
    {
        mutexObject.Lock();
        m_lValue += 1;
        mutexObject.Unlock();
    }

    int IntValue()
    {
        int value = 0;

        mutexObject.Lock();
        value = (int)m_lValue;
        mutexObject.Unlock();

        return value;
    }

    long LongValue()
    {
        long value = 0;

        mutexObject.Lock();
        value = (long)m_lValue;
        mutexObject.Unlock();

        return value;
    }

    void Set(long newValue)
    {
        mutexObject.Lock();
        m_lValue = newValue;
        mutexObject.Unlock();
    }

private:
    CMutex mutexObject;
    long m_lValue;

private:
    DISALLOW_COPY_AND_ASSIGN(AtomicLong);
};


class AtomicInteger
{
public:
    AtomicInteger(int initialValue = 0)
    {
        m_nValue = initialValue;
    }
    virtual ~AtomicInteger();

public:
    int  AddAndGet(int delta)
    {
        int value = 0;

        mutexObject.Lock();
        m_nValue += delta;
        value = m_nValue;
        mutexObject.Unlock();

        return value;
    }

    int DecrementAndGet()
    {
        int value = 0;

        mutexObject.Lock();
        m_nValue -= 1;
        value = m_nValue;
        mutexObject.Unlock();

        return value;
    }

    void Decrement()
    {
        mutexObject.Lock();
        m_nValue -= 1;
        mutexObject.Unlock();
    }

    double DoubleValue()
    {
        double value = 0;

        mutexObject.Lock();
        value = (double)m_nValue;
        mutexObject.Unlock();

        return value;
    }

    float FloatValue()
    {
        float value = 0;

        mutexObject.Lock();
        value = (float)m_nValue;
        mutexObject.Unlock();

        return value;
    }

    int Get()
    {
        int value = 0;

        mutexObject.Lock();
        value = m_nValue;
        mutexObject.Unlock();

        return value;
    }

    int GetAndAdd(int delta)
    {
        int value = 0;

        mutexObject.Lock();
        value = m_nValue;
        m_nValue += delta;
        mutexObject.Unlock();

        return value;
    }

    int GetAndDecrement()
    {
        int value = 0;

        mutexObject.Lock();
        value = m_nValue;
        m_nValue -= 1;
        mutexObject.Unlock();

        return value;
    }

    int GetAndIncrement()
    {
        int value = 0;

        mutexObject.Lock();
        value = m_nValue;
        m_nValue += 1;
        mutexObject.Unlock();

        return value;
    }

    int GetAndSet(int newValue)
    {
        int value = 0;

        mutexObject.Lock();
        value = m_nValue;
        m_nValue = newValue;
        mutexObject.Unlock();

        return value;
    }

    int IncrementAndGet()
    {
        int value = 0;

        mutexObject.Lock();
        m_nValue += 1;
        value = m_nValue;
        mutexObject.Unlock();

        return value;
    }

    void Increment()
    {
        mutexObject.Lock();
        m_nValue += 1;
        mutexObject.Unlock();
    }

    int IntValue()
    {
        int value = 0;

        mutexObject.Lock();
        value = (int)m_nValue;
        mutexObject.Unlock();

        return value;
    }

    long LongValue()
    {
        long value = 0;

        mutexObject.Lock();
        value = (long)m_nValue;
        mutexObject.Unlock();

        return value;
    }

    void Set(int newValue)
    {
        mutexObject.Lock();
        m_nValue = newValue;
        mutexObject.Unlock();
    }

private:
    CMutex mutexObject;
    int  m_nValue;

private:
    DISALLOW_COPY_AND_ASSIGN(AtomicInteger);
};

}




#endif /* SYNCOBJECT_H_ */
