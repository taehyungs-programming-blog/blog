---
layout: default
title: "05. Event Lock, Mutex Lock, ReadWrite Lock 구현해보기"
parent: "(기초)"
grand_parent: "(GameServer C# 🎯)"
great_grand_parent: "Legacy Archive"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## Event Lock

* 두 가지 구현방법이 존재
    * **Auto Reset Event** : 문(Signal)이 자동으로 잠긴다
    * **Manual Reset Event** : 문(Signal)이 수동으로 잠긴다
* `Set` - 누군가 진입이 가능한 상태를 의미
* `Reset` - 진입이 불가능한 상태를 의미
    * Auto Reset Event은 자동 Reset된다 생각하면 편하다

```csharp
class Lock
{
    AutoResetEvent _available = new AutoResetEvent(true/* 처음은 입장가능 */);   

    public void Acquire()
    {
        _available.WaitOne();   // 입장시도, 여기서 이벤트를 대기하게 됨.
        // Auto : 입장이 되면 자동으로 다음 입장은 막는다
    }

    public void Release()
    {
        _available.Set();
    }
}
```

* 이번엔 Manual로 구현해보자

```csharp
class Lock
{
    ManualResetEvent _available = new ManualResetEvent(true/* 처음은 입장가능 */);   

    public void Acquire()
    {
        _available.WaitOne();   // 입장시도
        _available.Reset();     // 문을 닫는다
        // 단, Wait와 Reset을 두 단계로 나누니 쓰레드 세이프 하지못함
        // 그럼 이걸 왜 쓰나?
        // 입장시도와 문을 닫는 행위를 별도로 해야할때 사용하면 된다.
    }

    public void Release()
    {
        _available.Set();
    }
}
```

---

## Mutex Lock

Mutex를 써보자

```csharp
class Program
{
    static int _num = 0;
    static Mutex _lock = new Mutex();

    static void Thread_1()
    {
        for(int i = 0; i < 100000; i++)
        {
            _lock.WaitOne();
            _num++;
            _lock.ReleaseMutex();
        }
    }

    static void Thread_2()
    {
        for(int i = 0; i < 100000; i++)
        {
            _lock.WaitOne();
            _num--;
            _lock.ReleaseMutex();
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

* 해보면 SpinLock보다 훨씬느리다
* EventLock처럼 커널단까지 가야해서인데
* 그럼 다시 `Event Lock`과 `Mutex Lock`는 무슨차이인가?

* Mutex는 내부적으로 많은 정보를 갖고있다.

1. 몇 번 락을 잡았는지
2. Mutex를 락잡은 쓰레드 정보

* 이런 정보를 갖고있기에 일반 SpinLock보다는 훨씬느리게 된다

---

## ReadWriteLock

* C#자체에서 ReaderWriterLock을 지원한다

```csharp
class Reward
{

}

static ReaderWriterLockSlim _lock3 = new ReaderWriterLockSlim();

static Reward GetRewardById(int id)
{
    // Read는 여러 쓰레드에서 가능하며
    // Write는 한 쓰레드에서만 가능
    _lock3.EnterReadLock();

    _lock3.ExitReadLock();
}

static void AddReward(Reward reward)
{
    _lock3.EnterWriteLock();

    _lock3.ExitWriteLock();
}
```