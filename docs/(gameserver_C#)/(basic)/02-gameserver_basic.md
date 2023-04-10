---
layout: default
title: "02. Multi Threading이 어려운 이유?"
parent: "(기초)"
grand_parent: "(GameServer C# 🎯)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## 컴파일러의 최적화 과정

```csharp
// Q. 아래 코드의 문제는 뭘까?

internal class Program
{
    static bool stop = false;

    static void MainThread()
    {
        Console.WriteLine("Thread Start");

        while(stop == false)
        {

        }

        Console.WriteLine("Thread End");
    }

    static void Main(string[] args)
    {
        Thread t = new Thread(MainThread);
        t.Start();

        Thread.Sleep(1000);
        stop = true;
        t.Join();
    }
}
```

```csharp
// 정답!?!?

static bool stop = false;

static void ThreadMain()
{
    Console.WriteLine("쓰레드 시작!");

    while(stop == false)
    {
        /*
    컴파일러 입장에서는 이렇게 최적화를 한다
    if(stop == false)
    {
        while(true)
        {}
    }
    컴파일러 입장에선 그럴만도 한게 내부에서 stop을 true로 바꿔줄 만한 요소가 없음.
    따라서 릴리즈 모드에서 무한루프에 빠지게 된다.
        */
    }

    Console.WriteLine("쓰레드 종료!");
}

static void Main(string[] args)
{
    Task t = new Task(ThreadMain);

    t.Start();

    Thread.Sleep(1000); // Thread가 시작할 수 있는 시간을 대기
    _stop = true;

    Console.WriteLine("Stop 호출");
    Console.WriteLine("종료 대기중");
    t.Join();
    Console.WriteLine("종료 성공");
}
```

---

### 해결해보자

```csharp
// ...

// volatile 지정하기
volatile static bool stop = false;
static void MainThread()
{
    Console.WriteLine("Thread Start");

    // ...
```

* 사실 이번강의는 `volatile`를 알려주려는 목적이 아니다.
    * 좀 더 정확히 말하면 `volatile`의 사용은 **추천하지 않는다**
    * `volatile`은 코드를 최적화 하지말라는 말이 아니라 **실시간으로 변수를 가져와** 달라는 명령이다.
    * 물론 C++에서도 `volatile`이 있지만 좀 다르게 동작(최적화 하지말라는 동작)
* 이번강의에서 하고싶은 말은 컴파일러에서 이러한 방법으로 최적화가 일어나고 따라서 **멀티쓰레드 환경에선 주의해서 코드를 짜야한다는 것**

---

## 캐시이론

컴퓨터의 구조는 대략적으로 이러하다

```

[[   CPU  ]]
[ 레지스터 ]
[ L1 캐시  ]
[ L2 캐시 ]
     |
     |
[[   RAM  ]]

```

* 그럼 무엇을 캐시에 우선적으로 넣어둘 것인가? 
    * **Temporal Locality** : 방금 호출된 메모리
    * **Spacial Locality** : 방금 호출된 메모리 근처의 메모리
* 위 두가지 캐싱 방법으로 인해 문제가 발생한다.
    * 만약 캐시에 올린 데이터가 멀티스레드 환경으로 인해 RAM의 데이터를 누군가 읽기 혹은 쓰기를 한다면??

### 테스트를 해보자.

* [Get Code 🌎](https://github.com/Arthur880708/CSharp_GameServer_Basic_Examples/tree/master/Thread_Basic/3.TheCache)

```csharp
int[,] arr = new int[10000, 10000];

{
    long now = DateTime.Now.Ticks;
    for(int y = 0; y < 10000; y++)
        for(int x = 0; x < 10000; x++)
            arr[y, x] = 1;
    long end = DateTime.Now.Ticks;
    Console.WriteLine($"(y, x) 순서 걸린 시간 {end - now}");
}

{
    long now = DateTime.Now.Ticks;
    for(int y = 0; y < 10000; y++)
        for(int x = 0; x < 10000; x++)
            arr[x, y] = 1;
    long end = DateTime.Now.Ticks;
    Console.WriteLine($"(x, y) 순서 걸린 시간 {end - now}");
}

/*

이론적으론 둘이 똑같은 시간이 나와야 하지만
(x, y)가 2배 이상오래 걸린다.
캐싱의 접근 방법때문이다.

*/
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/game-server/csharp/server-2-1.png"/>
</p>

---

## 메모리 배리어

* [Get Code 🌎]()

```csharp
static int x = 0;
static int y = 0;
static int r1 = 0;
static int r2 = 0;

static void Thread_1()
{
    y = 1;
    r1 = x;
}

static void Thread_2()
{
    x = 1;
    r2 = y;
}

static void Main(string[] args)
{
    int count = 0;
    while(true)
    {
        count++;
        x = y = r1 = r2 = 0;

        Task t1 = new Task(Thread_1);
        Task t2 = new Task(Thread_2);
        t1.Start();
        t2.Start();

        Task.WaitAll(t1, t2);

        // 과연 빠져나오는 경우가 있을까?
        if(r1 == 0 && r2 == 0)
         break;
    }

    // 생각보다 금방 빠져나온다 (보통 3~5번만에)
    Console.WriteLine($"{count}번만에 빠져나옴");
}
```

* 이게 가능한가???

```csharp
// CPU입장에서는 아래의 연산들이
static void Thread_1()
{
    // y = 1;과 r1 = x;연산의 연관성이 없기에 
    // r1 = x;를 먼저 실행하고 y = 1;을 실행하는 경우도 발생
    y = 1;
    r1 = x;
}

static void Thread_2()
{
    // 여기도 마찬가지
    x = 1;
    r2 = y;
}

// 그럼 r1, r2가 모두 0이 나올수 있다
```

* 이런식으로 CPU에서 마음대로 최적화를 해버리는데 이를 방지해 보자(->메모리 배리어)
* 메모리 배리어 역할
    * 코드재배치 억제
    * 가시성 향상

```csharp
static void Thread_1()
{
    y = 1;

    Thread.MemoryBarrier();
    // Thread.MemoryBarrier(); : Store, Load를 모두 막는다

    r1 = x;
}

static void Thread_2()
{
    x = 1;

    Thread.MemoryBarrier();

    r2 = y;
}
```

* 사실 `Thread.MemoryBarrier()`를 직접적으로 **쓰게 되는 경우는 드물다**
* 이런식으로 메모리 배리어를 두고, **`Lock`등이 구현된다고 알고있자** -> 결국 Lock을 쓴다는 말.

마지막으로 예제하나만 보고 넘어가자

```csharp
int _answer;
bool _complete;

void A()
{
    _answer = 123;
    Thread.MemoryBarrier(); // 여기는 왜 사용되는지 알겠고
    _complete = true;
    Thread.MemoryBarrier(); 
    // 두 번째 배리어의 경우 
    // _complete = true; 이후 Flush 메모리에 올리는 것을 진행해 달라는 요청
}

void B()
{
    // 여기 배리어의 경우
    // if문의 _complete를 읽기위해서 최신의 메모리를 읽어와 달라는 요청
    Thread.MemoryBarrier();
    if(_complete)
    {
        // 여기는 상동
        Thread.MemoryBarrier();
        Console.WriteLine(_answer);
    }
}
```

---

## Context Switching

```csharp
class SpinLock
{
    volatile int _locked = 0;

    public void Acquire()
    {
        while(true)
        {
            int expected = 0;
            int desire = 1;
            int original = Interlocked.CompareExchange(ref _locked, desire, expected);
            if(original == 0)
                break;

            // 쓰레드를 쉬게 하는 방법은 세 가지가 있다
            Thread.Sleep(1);    // 무조건 1ms를 쉬어라
            Thread.Sleep(0);    // 조건부 양보 -> 자신보다 우선순위가 낮은 쓰레드에게는 양보 불가
            Thread.Yield();     // 관대한 양보 -> 조건없이 쓰레드 우선권 양보
        }
    }

    public void Release()
    {
        _locked = 0;
    }
}
```

* Context Switching 
    * 쓰레드의 권한이 이전되며 필요한 데이터를 캐시에 저장하는데
    * 그 필요한 데이터 복사 시간이 필요해진다.
    * 따라서 너부 빈번한 쓰레드 권한 이전은 오히려 단점이 된다