---
layout: default
title: "9. Thread Class"
parent: (C# Adv)
grand_parent: "(C#)"
great_grand_parent: "Legacy Archive"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## C#에서 Thread를 만드는 방법

* Thread Class
* Thread Pool
* Task
* Async IO
* async, await

---

## 최적의 Thread수?

* [참고 사이트 🌍](https://junshock5.tistory.com/147)

* 사실상 정해진것은 없음.
* **적정 스레드 개수 = CPU 수 * (1 + 대기 시간/서비스 시간)**
    * 대기 시간이 길어지면 -> 스레드가 늘어나게 된다.
    * 서비스 시간이 길어지면 -> 스레드가 줄어들게 된다.

---

## Thread Class

```csharp
using System;
using System.Threading;

class Program
{
    public static void Foo()
    {
        for (int i = 0; i < 10000; i++)
            Console.Write("1");
    }
    public static void Main()
    {
        Thread t = new Thread(Foo);
        t.Start();

        for (int i = 0; i < 10000; i++)
        {
            Console.Write("2");
        }
    }
}
```

```csharp
using System;
using System.Threading;

class Program
{
    public static void F1()             { Console.WriteLine($"F1"); }
    public static void F2(object? obj)  { Console.WriteLine($"F2 : {obj.ToString()}"); }
    public static void F3(object obj)   { Console.WriteLine($"F3 : {obj.ToString()}"); }
    public static void F4(string msg)   { Console.WriteLine($"F4 : {msg}"); }
    public static void F5(int a, int b) { Console.WriteLine($"F5 : {a}, {b}"); }

    public static void Main()
    {
        Thread t1 = new Thread(F1); t1.Start();

        Thread t2 = new Thread(F2); t2.Start("Hello");
        Thread t3 = new Thread(F3); t3.Start("Hello");
        //Thread t4 = new Thread(F4); // error 

        Thread t4 = new Thread(() => F4("Hello"));
        t4.Start();

        Thread t5 = new Thread((arg) => F4((string)arg));
        t5.Start("Hello");

        Thread t6 = new Thread(() => F5(1,2));
        t6.Start();
    }
}
```

```csharp
using System;
using System.Threading;

class Program
{
    static void Foo(int n)
    {
        Console.WriteLine(n);
    }
    public static void Main()
    {
        for (int i = 0; i < 20; i++)
        {
            //Thread t = new Thread(() => Foo(i));
            // 이렇게 람다 캡쳐를 사용할 경우, 중복된 값이 나타나게 된다. 
                // 이유는 이후에 설명
            int temp = i;
            Thread t = new Thread(() => Foo(temp));
            t.Start();
        }
    }
}
```

---

## Thread Memebers

```csharp
using System;
using System.Threading;

class Program
{
    public static void Foo()
    {
        // 자신의 참조가 필요하면
        Thread t = Thread.CurrentThread;
        Console.WriteLine($"{t.ManagedThreadId}");

        Console.WriteLine("Foo");
        Thread.Sleep(2000);
    }

    public static void Main()
    {
        Thread t1 = new Thread(Foo);
        t1.Start();

        t1.Name = "AAA";
        //t1.Name = "BBB"; // runtime rror

        Console.WriteLine($"{t1.IsAlive}");
        Console.WriteLine($"{t1.ManagedThreadId}");

        t1.Join();

    }
}
```

```csharp
using System;
using System.Threading;

class Program
{
    public static void Foo(string s, int ms)
    {
        Console.WriteLine($"{s} Start");
        Thread.Sleep(ms);
        Console.WriteLine($"{s} Finish");
    }

    public static void Main()
    {
        Thread t1 = new Thread(() => Foo("A", 3000));
        t1.IsBackground = false; // foreground
        t1.Start();

        Thread t2 = new Thread(() => Foo("B", 9000));
        t2.IsBackground = true; // background
        t2.Start();

        Thread t3 = new Thread(() => Foo("C", 7000));
        t3.IsBackground = false; // foreground
        t3.Start();

        Thread t4 = new Thread(() => Foo("D", 5000));
        t4.IsBackground = true; // background
        t4.Start();
        // 주 스레드가 종료!!
    }
}
```

* 프로세스의 종료는 **프로세스 내의 모든 Foreground** 스레드가 종료될 때이다.

---

## Thread 취소하기 (Cooperative Cancellation)

* Cancellation Token을 이용해 한다.

```csharp
using System;
using System.Threading;

class Program
{
    public static void Count(int cnt)
    {
        for (int i = 0; i < cnt; i++)
        {
            Console.WriteLine(i);
            Thread.Sleep(200);
        }
    }
    public static void Main()
    {
        Thread t = new Thread(o => Count(1000));
        t.Start();

    }
}
```

```csharp
using System;
using System.Threading;

class Program
{
    public static void Count(CancellationToken token,  int cnt)
    {
        for (int i = 0; i < cnt; i++)
        {
            if ( token.IsCancellationRequested)
            {
                Console.WriteLine("Cancelling");
                break;
            }
            Console.WriteLine(i);
            Thread.Sleep(200);
        }
        if (token.IsCancellationRequested)
        {
            Console.WriteLine("Cancelled");
        }
        else
            Console.WriteLine("Finish Count");

    }


    public static void Main()
    {
        CancellationTokenSource cts = new CancellationTokenSource();

        // 취소하게 만들기
        //Thread t = new Thread(o => Count(cts.Token, 1000));

        // 취소할수 없는 토큰
        Thread t = new Thread(o => Count(CancellationToken.None, 1000));
        t.Start();

        Console.ReadLine();
        cts.Cancel();
    }
}
```

```csharp
using System;
using System.Threading;

class Program
{
    public static void Count(CancellationToken token, int cnt)
    {
        for (int i = 0; i < cnt; i++)
        {
            if (token.IsCancellationRequested)
            {
                Console.WriteLine("Cancelling");
                break;
            }
            Console.WriteLine(i);
            Thread.Sleep(200);
        }
        if (token.IsCancellationRequested)
        {
            Console.WriteLine("Cancelled");
        }
        else
            Console.WriteLine("Finish Count");

    }


    public static void Main()
    {
        CancellationTokenSource cts = new CancellationTokenSource();

        // 취소가 호출될 시 여기로 들어온다.
        CancellationTokenRegistration m1 = cts.Token.Register(() => Console.WriteLine("Cancelled 1"));
        cts.Token.Register(() => Console.WriteLine("Cancelled 2"));

        m1.Dispose(); // 등록된 함수 제거.

        Thread t = new Thread(o => Count(cts.Token, 1000));
        t.Start();

        // 2초뒤에 취소해주세요
        cts.CancelAfter(2000);


        Console.ReadLine();
        //cts.Cancel();
    }
}
```

```csharp
using System;
using System.Threading;

class Program
{
    public static void Count(CancellationToken token, int cnt)
    {
        for (int i = 0; i < cnt; i++)
        {
            if (token.IsCancellationRequested)
            {
                Console.WriteLine("Cancelling");
                break;
            }
            Console.WriteLine(i);
            Thread.Sleep(200);
        }
        if (token.IsCancellationRequested)
        {
            Console.WriteLine("Cancelled");
        }
        else
            Console.WriteLine("Finish Count");

    }


    public static void Main()
    {
        // 토큰 둘 중 하나라도 취소를 할 경우 취소 동작하게 만들기
        CancellationTokenSource cts1 = new CancellationTokenSource();
        cts1.Token.Register(() => Console.WriteLine("Cancel 1"));

        CancellationTokenSource cts2 = new CancellationTokenSource();
        cts2.Token.Register(() => Console.WriteLine("Cancel 2"));

        CancellationTokenSource cts = CancellationTokenSource.CreateLinkedTokenSource(cts1.Token,
                                        cts2.Token);


        Thread t = new Thread(o => Count(cts.Token, 1000));
        t.Start();

        Console.ReadLine();
        cts2.Cancel();
    }
}
```