---
layout: default
title: "03. Interlocked, lock, dead lock"
parent: "(기초)"
grand_parent: "(GameServer C# 🎯)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌎](https://github.com/Arthur880708/CSharp_GameServer_Basic_Examples/tree/master/Thread_Basic/5.ThreadTest)

```csharp
static volatile int number = 0;

static void Thread_1()
{
    for(int i = 0; i < 100000; i++)
        number++;
}

static void Thread_2()
{
    for(int i = 0; i < 100000; i++)
        number--;
}

static void Main(string[] args)
{
    Task t1 = new Task(Thread_1);
    Task t2 = new Task(Thread_2);
    t1.Start();
    t2.Start();

    Task.WaitAll(t1, t2);

    // 여기서 0이 나오지 않게 된다.
    Console.WriteLine(number);
}
```

* 이게 말이 되나?

* 왜 그럴까?
    * `number++`동작을 할때 ecx레지스터에 복사 후 1을 증가하고 다시 복사하는 과정을 거친다.
* 마치 아래와 같이 동작한다

```csharp
int temp = number;
temp += 1;
number = temp;
```

* 이러한 동작속에서 우리가 생각했던 값과 다른 값이 나타나게 된다.
* 이런 문제를 아토믹하지 못해서 생기는 문제라 말하며...

* 해결해보자.

```csharp
static void Thread_1()
{
    for(int i = 0; i < 100000; i++)
        // 이렇게 해서 아토믹한 연산을 가능하게 만든다.
        Interlocked.Increament(ref number);
        // 단, 이 동작은 성능이 나쁘다
}

static void Thread_2()
{
    for(int i = 0; i < 100000; i++)
        Interlocked.Decrement(ref number);
}
```

```csharp
static void Thread_1()
{
    for(int i = 0; i < 100000; i++)
        int ret = Interlocked.Increament(ref number);
        // 참고로 Interlocked.Increament은 리턴을 받는데
        // 리턴을 받는이유는 number가 현재 어떤값인지 알려주기 위함이다.
        // ??? 그냥 number를 브레이크포인트에서 찍어보면 안되나?? -> 아주 싱글스레드 적인마인드
        // number를 다른 스레드에서 언제접근중일지 모름
}
```

---

## Interlocked를 좀 더 간단히 써보자 -> Lock

```csharp
static int number = 0
static object _obj = new object();

static void Thread_1()
{
    for(int i = 0; i < 100000; i++)
    {
        Monitor.Enter(_obj);    // Mutex Lock과 동일

        number++;

        Monitor.Exit(_obj);     // Unlock
    }
}

static void Thread_2()
{
    for(int i = 0; i < 100000; i++)
    {
        Monitor.Enter(_obj);

        number--;

        // 이런 문제가 발생한다. Exit를 해주지 않은 상태에서
        // 누군가 return을 해버린다면 ???
        // 무한대기(데드락)이 발생한다
        return;

        Monitor.Exit(_obj);
    }
}
```

* 무한대기(데드락) 문제를 해결해보자

```csharp
static void Thread_2()
{
    for(int i = 0; i < 100000; i++)
    {
        // lock으로 해결가능
        lock(_obj)
        {
            number--;
        }
    }
}
```

---

## Dead Lock

* 두 쓰레드가 락을 잡고 서로 락이 풀리기를 대기한다면??? -> 또 다른 데드락이 발생

```csharp
class SessionManager
{
    // 클래스 각각 자신의 lock이 있다고 가정하자
    static object _lock = new object();

    public static void Test()
    {
        lock(_lock)
        {
            // 내가 lock을 잡은 상태로 다른 클래스의 lock잡는 함수를 호출
            UserManager.TestUser();
        }
    }

    public static void TestSession()
    {
        lock(_lock)
        {

        }
    }

    // ...
}

class UserManager
{
    static object _lock = new object();

    public static void Test()
    {
        lock(_lock)
        {
            SessionManager.TestSession();
        }
    }

    public static void TestUser()
    {
        lock(_lock)
        {

        }
    }

    // ...
}

// 여기서 주의할 점은 Thread의 타이밍이 조금만 어긋나도 이러한 데드락은 발생하지 않는다
// 오히려 이런 부분이 데드락을 잡기를 어렵게 하는 부분이다.
static void Thread_1()
{
    for(int i = 0; i < 100000; i++)
    {
        SessionManager.Test();
    }
}

static void Thread_2()
{
    for(int i = 0; i < 100000; i++)
    {
        UserManager.Test();
    }
}
```

* 해결방법이 있나??
    * 사실 일정시간을 대기하다가 lock을 못잡을 시 알려주는 코드가 있지만
    * 위험을 피해가는 것이 불과하다.

* 오히려 이런 데드락이 발생하면 lock을 순서를 바꾸는 방향으로 해결하는게 나음.
* 따라서 데드락이 발견되고 해결하는 방향이 옳을 수 있음 **(미리 예방이 거의 불가능하다)**