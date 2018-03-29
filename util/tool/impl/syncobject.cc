//#include "stdafx.h"
#include "../syncobject.h"


namespace toolHelp
{

/********************************************************************
  线程互斥类
********************************************************************/
CMutex::CMutex()
{
    if(0 != pthread_mutexattr_init(&mutexAttribute))
    {
        isInitialized = false;
        return;
    }

    if(0 != pthread_mutex_init(&mutexHandle, &mutexAttribute))
    {
        pthread_mutexattr_destroy(&mutexAttribute);
        isInitialized = false;
        return;
    }
    isInitialized = true;
}

CMutex::~CMutex()
{
    if(isInitialized)
    {
        pthread_mutexattr_destroy(&mutexAttribute);
        pthread_mutex_destroy(&mutexHandle);
        isInitialized = false;
    }
}

pthread_mutex_t* CMutex::GetMutexHandle()
{
    return &mutexHandle;
}

int32 CMutex::Lock()
{
    return pthread_mutex_lock(&mutexHandle);
}

int32 CMutex::Unlock()
{
    return pthread_mutex_unlock(&mutexHandle);
}


int32 CMutex::TryLock()
{
    return pthread_mutex_trylock(&mutexHandle);
}





/********************************************************************
  线程读写锁
********************************************************************/
CReadWriteMutex::CReadWriteMutex()
{
    if(0 != pthread_rwlockattr_init(&readWriteMutexHandleAttr))
    {
        isInitialized = false;
        return;
    }

    if(0 != pthread_rwlock_init(&readWriteMutexHandle, &readWriteMutexHandleAttr))
    {
        pthread_rwlockattr_destroy(&readWriteMutexHandleAttr);
        isInitialized = false;
        return;
    }

    isInitialized = true;
}

CReadWriteMutex::~CReadWriteMutex()
{
    if(isInitialized)
    {
        pthread_rwlockattr_destroy(&readWriteMutexHandleAttr);
        pthread_rwlock_destroy(&readWriteMutexHandle);
        isInitialized = false;
    }
}

pthread_rwlock_t* CReadWriteMutex::GetReadWriteMutexHandle()
{
    return &readWriteMutexHandle;
}

int32 CReadWriteMutex::LockRead()
{
    return pthread_rwlock_rdlock(&readWriteMutexHandle);
}

int32 CReadWriteMutex::LockWrite()
{
    return  pthread_rwlock_wrlock(&readWriteMutexHandle);
}

int32 CReadWriteMutex::Unlock()
{
    return pthread_rwlock_unlock(&readWriteMutexHandle);
}


int32 CReadWriteMutex::TryLock()
{
    return pthread_rwlock_tryrdlock(&readWriteMutexHandle);
}


int32 CReadWriteMutex::Lock()
{
    return pthread_rwlock_rdlock(&readWriteMutexHandle); //缺省是读锁
}




/********************************************************************
  线程条件类
********************************************************************/
CCondition::CCondition()
{
    isSendSignal = false;
    activeWaiters = 0;

    pthread_condattr_t condattrDetails;

    memset(&condattrDetails, 0, sizeof(condattrDetails));

    if(0 != pthread_condattr_init(&condattrDetails))
    {
        isInitialized = false;
        return;
    }

    if(0 != pthread_cond_init(&conditionHandle, &condattrDetails))
    {
        isInitialized = false;
        return;
    }

    if(0 != pthread_condattr_destroy(&condattrDetails))
    {
        isInitialized = false;
        return;
    }

    isInitialized = true;
}

CCondition::~CCondition()
{
    uint32 retryCount = 0;

    while ((EBUSY == pthread_cond_destroy(&conditionHandle)) && (retryCount <= 10))
    {
        retryCount++;
        SignalAll();
    }

    isSendSignal = false;
    activeWaiters = 0;
    isInitialized = false;
}

void CCondition::Wait(void)
{
    Lock();

    ++activeWaiters;

    while (!isSendSignal)
    {
        pthread_cond_wait(&conditionHandle, mutexObject.GetMutexHandle());
    }

    --activeWaiters;

    if (0 == activeWaiters)
    {
        isSendSignal = false;
    }

    Unlock();
}

bool CCondition::WaitTimeout(int ms)
{
    struct timespec timeout;
    struct timeval  tp;
    bool isTimeOut = false;

    gettimeofday(&tp, NULL);

    timeout.tv_sec = (ms / 1000) + tp.tv_sec;
    timeout.tv_nsec = ((ms % 1000) * 1000000) + (tp.tv_usec * 1000);

    while (timeout.tv_nsec >= 1000000000)
    {
        timeout.tv_nsec -= 1000000000;
        ++timeout.tv_sec;
    }

    Lock();

    ++activeWaiters;

    int32 result = 0;

    while (!isSendSignal)
    {
        if (ETIMEDOUT == (result = pthread_cond_timedwait(&conditionHandle, mutexObject.GetMutexHandle(), &timeout)))
        {
            isTimeOut = true;
            break;
        }
    }

    --activeWaiters;

    if ((0 == result) &&
        (0 == activeWaiters))
    {
        isSendSignal = false;
    }

    Unlock();
    return !isTimeOut; //if timeout return false, else true
}

void CCondition::Signal(void)
{
    Lock();

    if (!isSendSignal)
    {
        pthread_cond_signal(&conditionHandle);
        isSendSignal = true;
    }

    Unlock();
}

void CCondition::SignalAll(void)
{
    Lock();

    pthread_cond_broadcast(&conditionHandle);
    isSendSignal = true;

    Unlock();
}

int32 CCondition::Lock(void)
{
    return mutexObject.Lock();
}

int32 CCondition::Unlock(void)
{
    return mutexObject.Unlock();
}





/********************************************************************
  线程自旋锁
********************************************************************/
CSpinLock::CSpinLock()
{
    if(0 != pthread_spin_init(&spinLockHandle, 0))
    {
        isInitialized = false;
    }
    else
    {
        isInitialized = true;
    }
}

CSpinLock::~CSpinLock()
{
    if(isInitialized)
    {
        pthread_spin_destroy(&spinLockHandle);
        isInitialized = false;
    }
}

pthread_spinlock_t* CSpinLock::GetSpinLockHandle()
{
    return &spinLockHandle;
}

int32 CSpinLock::Lock()
{
    return pthread_spin_lock(&spinLockHandle);
}

int32 CSpinLock::Unlock()
{
    return pthread_spin_unlock(&spinLockHandle);
}

int32 CSpinLock::TryLock()
{
    return pthread_spin_trylock(&spinLockHandle);
}



/********************************************************************
  线程信号量
********************************************************************/
CSemaphore_1::CSemaphore_1(uint32 initialCount)
{
    if(0 != sem_init(&semaphoreHandle, 0, initialCount))
    {
        isInitialized = false;
    }
    else
    {
        isInitialized = true;
    }
}

CSemaphore_1::~CSemaphore_1()
{
    if(isInitialized)
    {
        sem_destroy(&semaphoreHandle);
        isInitialized = false;
    }
}

int32 CSemaphore_1::Lock()
{
    return sem_wait(&semaphoreHandle);
}

int32 CSemaphore_1::Wait()
{
    return sem_wait(&semaphoreHandle);
}

int32 CSemaphore_1::TryLock(uint32 milliseconds)
{
    const struct timespec timeOut = {0, milliseconds * 1000 * 1000 };
    return sem_timedwait(&semaphoreHandle, &timeOut);
}

int32 CSemaphore_1::Unlock()
{
    return sem_post(&semaphoreHandle);
}

int32 CSemaphore_1::Post()
{
    return sem_post(&semaphoreHandle);
}

int32 CSemaphore_1::Peek()
{
    int32 semaphoreValue;
    if(0 != sem_getvalue(&semaphoreHandle, &semaphoreValue))
    {
        return -1;
    }
    return semaphoreValue;
}


}

