---
layout: default
title: "07. TLS(Thread Local Storage)"
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

* Thread Local Storage : 쓰레드별로 별도의 변수(데이터)를 관리해보자
    * 그런데 왜 모든 데이터를 공용으로 관리하면 안될까?
    * 효율성이 떨어진다.
    * 특정순간에 모든 쓰레드가 한 공용데이터로 몰린다고 가정해보자
    * 마치 싱글쓰레드 처럼 동작하게 된다

* 그럼 쓰레드마다 할당되는 스택에 할당하나??
    * 또 그것도 아니다. 스택은 함수의 호출시에 사용되는 불안정한 메모리이다. 이런용도는 아님

```csharp
static ThreadLocal<string> ThreadName = new ThreadLocal<string>();

static void WhoAmI()
{
    ThreadName.Value = &"My Name Is {Thread.CurrentThread.ManagedThreadId}";

    Thread.Sleep(1000);

    Console.WriteLine(ThreadName.Value);
}

static void Main(string[] args)
{
    /*
    // 쓰레드풀을 제한을 두면 그래도 같은 이름을 사용하는 쓰레드가 발생한다
    ThreadPool.SetMinThreads(1,1);
    ThreadPool.SetMinThreads(3,3);
    */
    Parallel.Invoke(WhoAmI, WhoAmI, WhoAmI, WhoAmI, WhoAmI, WhoAmI);
}
```

```csharp
// ThreadName이 할당되었는지 확인 후 할당
static ThreadLocal<string> ThreadName = new ThreadLocal<string>(() => {return &"My Name Is {Thread.CurrentThread.ManagedThreadId}"; });

static void WhoAmI()
{
    bool repeat = ThreadName.IsValueCreated;
    if(repeat)
        Console.WriteLine(ThreadName.Value + " (repeat)");
    else
        Console.WriteLine(ThreadName.Value);

    // ThreadName.Dispose();    // Dispose로 제거가능
}
```

