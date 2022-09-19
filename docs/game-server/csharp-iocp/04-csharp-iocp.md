---
layout: default
title: "04. SpinLock 구현해보기"
parent: "(C# IOCP)"
grand_parent: "Game Server 👾"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## 1차 구현

```csharp
class SpinLock
{
    volatile bool _locked = false;

    public void Acquire()
    {
        while(_locked)
        {
            // 잠김이 풀리기를 대기
        }

        _locked = true;
    }

    public void Release()
    {
        _locked = false;
    }
}

class Program
{
    static int _num = 0;
    static SpinLock _lock = new SpinLock();

    static void Thread_1()
    {
        for(int i = 0; i < 100000; i++)
        {
            _lock.Acquire();
            _num++;
            _lock.Release();
        }
    }

    static void Thread_2()
    {
        for(int i = 0; i < 100000; i++)
        {
            _lock.Acquire();
            _num--;
            _lock.Release();
        }
    }

    static void Main(string[] args)
    {
        Task t1 = new Task(Thread_1);
        Task t2 = new Task(Thread_2);
        t1.Start();
        t2.Start();

        Task.WaitAll(t1, t2);
    }
}
```

너무 쉬운데???<br>
하지만... 동작해 보면 `_num`이 0이 안나온다<br>

왜지?

```csharp
class SpinLock
{
    volatile bool _locked = false;

    public void Acquire()
    {
        // 두 쓰레드가 거의 동시에 while문에 도착
        while(_locked)
        {
            
        }

        // 그럴경우 두 쓰레드가 모두 락을 획득하게 된다.
        _locked = true;

        /*
            근본적으로 이런 문제가 일어난 원인은
            락을 확인하는 while과 락을 잠그는 _locked = true를
            한 동작으로 해야지 따로따로 해서 발생한 문제이다
            어려운 말로는 아토믹하게 처리해보자
        */
    }

    public void Release()
    {
        _locked = false;
    }
}
```

```csharp
class SpinLock
{
    volatile int _locked = 0;

    public void Acquire()
    {
        while(true)
        {
            int original = Interlocked.Exchange(ref _locked, 1)
            if(original == 0)
            {
                // _locked를 1로 바꿔 주세요
                // 그리고 original(_locked)이 0인지 확인
                break;
            }
        }
    }

    public void Release()
    {
        _locked = 0;
    }
}
```

이번엔 다른 버전으로 개발해보자

```csharp
class SpinLock
{
    volatile int _locked = 0;

    public void Acquire()
    {
        while(true)
        {
            // CAS(Comapare And Swap)
            int original = Interlocked.CompareExchange(ref _locked, 1, 0);
            if(original == 0)
                break;
            // _locked이 0일 경우 1을 넣어주세요
        }
    }

    public void Release()
    {
        _locked = 0;
    }
}
```

이렇게 구현할 경우 조금 헷갈리니 변수를 써주는 방향으로 구현을 많이 한다.

```csharp
class SpinLock
{
    volatile int _locked = 0;

    public void Acquire()
    {
        while(true)
        {
            // CAS(Comapare And Swap)
            int expected = 0;
            int desire = 1;
            int original = Interlocked.CompareExchange(ref _locked, desire, expected);
            if(original == 0)
                break;
        }
    }

    public void Release()
    {
        _locked = 0;
    }
}
```
