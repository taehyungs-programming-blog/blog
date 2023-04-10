---
layout: default
title: "01. 프로젝트 설정"
parent: "(기초)"
grand_parent: "(GameServer C# 🎯)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## 환경설정

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/game-server/csharp/server-1-1.png"/>
</p>

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/game-server/csharp/server-1-2.png"/>
</p>

---

## Thread

* [Get Code 🌎](https://github.com/Arthur880708/CSharp_GameServer_Basic_Examples/tree/master/Thread_Basic/1.HelloThread)

```csharp
// Thread 생성
static void MainThread()
{
    Console.WriteLine("Hello Thread!");
}

static void Main(string[] args)
{
    Thread t = new Thread(MainThread);
    t.Name = "Test Thread"; // Thread 이름도 지정가능
    t.Start();
}
```

---

### Thread Fore/Back ground

```csharp
// C#의 Thread는 Main이 종료되어도 같이 종료되지 않음
static void MainThread()
{
    while(true)
    {
        // 계속 Hello Thread가 출력됨
        Console.WriteLine("Hello Thread!");
    }
}

static void Main(string[] args)
{
    Thread t = new Thread(MainThread);
    //t.IsBackground = true;       // true로 변경시 Main과 같이 죽게된다.
    t.Start();
}
```

---

### Join

```csharp
// Thread 생성
static void MainThread()
{
    Console.WriteLine("Hello Thread!");
}

static void Main(string[] args)
{
    Thread t = new Thread(MainThread);
    t.Start();
    t.Join();   // Thread의 종료를 대기한다.
}
```

---

### Thread를 만들지 말고 간단한 비동기 작업을 처리하고자 한다면? => ThreadPool

```csharp
// Thread 생성
static void MainThread(object state)
{
    for(int i = 0; i < 5; i++)
        Console.WriteLine("Hello Thread!");
}

static void Main(string[] args)
{
    ThreadPool.QueueUseWorkItem(MainThread);

    // 참고로 ThreadPool은 생명주기가 Main과 같기에 
    // 그냥 실행시 Main이 종료되며 자동으로 MainThread가 다 돌지않고 종료됨

    while(true)
    {
        // 따라서 while로 잠깐 잡아두자
    }
}
```

* 그런데 `ThreadPool`을 사용하는 이유가 있을까?
    * `Thread`는 무겁기 때문에 오히려 `ThreadPool`를 사용하는게 유리할 수 있다
    * 따라서 차라리 Thread관리를 컴파일러에게 맡기는게 나을 수 있음
    * 그럼 `ThreadPool`는 만능인가?
    * 또 그렇지도 않은게 각 Thread에서 작업(연산량)이 많다면 오히려 `ThreadPool`이 느릴수 있음
    * 상황에 맞게 잘 사용하자
    * 여기서 상황인란 `ThreadPool`은 될 수 있으면 짧은 비동기 작업을 수행

### ThreadPool 사용법

```csharp
// Thread 생성
static void MainThread(object state)
{
    for(int i = 0; i < 5; i++)
        Console.WriteLine("Hello Thread!");
}

static void Main(string[] args)
{
    ThreadPool.SetMinThreads(
        1 /* workerThreads */, 
        1 /* completionPortThreads */
        );
    // completionPortThreads : (network 등) InputOutput Thread 설정
    ThreadPool.SetMaxThreads(5, 5);

    for(int i = 0; i < 5; i++)
        ThreadPool.QueueUseWorkItem((obj) => {while(true) {}}); 
    // 무한으로 5개의 thread를 잡아보자

    // 여기 Thread가 돌아갈까?
    // -> 안돌아감
    ThreadPool.QueueUseWorkItem(MainThread);
```

* 위와 같이 특정 Thread가 ThreadPool을 잡고있을 수 있는 문제점을 해결해 보자

```csharp
static void MainThread(object state)
{
    for(int i = 0; i < 5; i++)
        Console.WriteLine("Hello Thread!");
}

static void Main(string[] args)
{
    Task t = new Task((obj) => {while(true) {}}, 
                TaskCreationOptions.LongRunning /* 오래걸리는 작업이라고 알려준다 */);
    // TaskCreationOptions.LongRunning을 옵션으로 넣을경우 별도의 Thread에서 관리해준다

    t.Start();

    // 정상동작함.
    ThreadPool.QueueUseWorkItem(MainThread);
}
```