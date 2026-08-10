---
layout: default
title: "06. ReadWriteLock 직접구현"
parent: "(C# IOCP)"
grand_parent: "Game Server 👾"
great_grand_parent: "Legacy Archive"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

`ReaderWriterLock`을 직접구현해보자

```csharp
// Lock.cs

// 여기서는 재귀적 락은 허용하지 않음
// 스핀락의 경우 5000번 이후 Yield
class Lock
{
    const int EMPTY_FLAG = 0x00000000;
    const int WRITE_FLAG = 0x7FFF0000;
    const int READ_FLAG = 0x0000FFFF;
    const int MAX_SPIN_COUNT = 5000;

    // [Unused(1비트)] [WriteThreadId(15비트)] [ReadCount(16비트)]
    int _flag = EMPTY_FLAG;

    public void WriteLock()
    {
        int desired = (Thread.CurrentThread.ManagedThreadId << 16) & WRITE_FLAG;
        while(true)
        {
            for(int i = 0; i < MAX_SPIN_COUNT; i++)
            {
                /*
                if(_flag == EMPTY_FLAG)
                    _flag = desired;
                */
                if( Interlocked.CompareExchange(ref _flag, desired, EMPTY_FLAG) == EMPTY_FLAG)
                    return;
            }

            Thread.Yield();
        }
    }

    public void WriteUnlock()
    {
        Interlocked.Exchange(ref _flag, EMPTY_FLAG);
    }

    public void ReadLock()
    {
        while(true)
        {
            for(int i = 0; i < MAX_SPIN_COUNT; i++)
            {
                /*
                if((_flag & WRITE_MASK) == 0)
                {
                    _flag = _flag + 1;
                    return;
                }
                */
                int expected = (_flag & READ_MASK);
                if(Interlocked.CompareExchange(ref _flag, expected + 1, expected) == expected)
                    return;
            }

            Thread.Yield();
        }
    }

    public void ReadUnlock()
    {
        Interlocked.Decrement(ref _flag);
    }
}
```

재귀적 락을 허용하는 버전을 구현해보자

```csharp
// Lock.cs

// 단, WriteLock에서 WriteLock, ReadLock을 재귀적으로 잡는 것은 지원
// ReadLock에서 WriteLock을 재귀적으로 잡는 것은 지원안함

// 스핀락의 경우 5000번 이후 Yield
class Lock
{
    const int EMPTY_FLAG = 0x00000000;
    const int WRITE_FLAG = 0x7FFF0000;
    const int READ_FLAG = 0x0000FFFF;
    const int MAX_SPIN_COUNT = 5000;

    // [Unused(1비트)] [WriteThreadId(15비트)] [ReadCount(16비트)]
    int _flag = EMPTY_FLAG;
    int _writeCount = 0;

    public void WriteLock()
    {
        // 동일 쓰레드가 WriteLock을 획득하는지 확인
        int lockThreadId = (_flag & WRITE_MASK) >> 16;
        if(Thread.CurrendThread.ManagedThreadId == lockThreadId)
        {
            _writeCount++;
            return;
        }

        int desired = (Thread.CurrentThread.ManagedThreadId << 16) & WRITE_FLAG;
        while(true)
        {
            for(int i = 0; i < MAX_SPIN_COUNT; i++)
            {
                if( Interlocked.CompareExchange(ref _flag, desired, EMPTY_FLAG) == EMPTY_FLAG)
                {
                    _writeCount = 1;
                    return;
                }
            }

            Thread.Yield();
        }
    }

    public void WriteUnlock()
    {
        int lockCount = --_writeCount;
        if(lockCount == 0)
            Interlocked.Exchange(ref _flag, EMPTY_FLAG);
    }

    public void ReadLock()
    {
        int lockThreadId = (_flag & WRITE_MASK) >> 16;
        if(Thread.CurrendThread.ManagedThreadId == lockThreadId)
        {
            Interlocked.Increment(ref _flag);
            return;
        }

        while(true)
        {
            for(int i = 0; i < MAX_SPIN_COUNT; i++)
            {
                int expected = (_flag & READ_MASK);
                if(Interlocked.CompareExchange(ref _flag, expected + 1, expected) == expected)
                    return;
            }

            Thread.Yield();
        }
    }

    public void ReadUnlock()
    {
        Interlocked.Decrement(ref _flag);
    }
}
```
