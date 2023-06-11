---
layout: default
title: "11. Task"
parent: (C# Adv)
grand_parent: "(C#)"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## Task

```csharp
using System;
using System.Threading;

class Program
{    
    public static void Foo(object arg)
    {
        Console.WriteLine("Foo");
        //return 100;
    }

    public static void Main()
    {
        Thread t1 = new Thread(Foo);
        t1.Start("Hello");
        t1.Join(); // 스레드 종료 대기.

        // ThreadPool은 종료를 알수없다.(대기불가능)
        // Thread, ThreadPool은 스레드의 반환값을 알수 없다.
        // Thread, ThreadPool은 연속된 실행이 불가능하다.
            // Task의 탄생
        ThreadPool.QueueUserWorkItem(Foo, "Hello");    
    }
}
```

```csharp
using System;
using System.Threading;
using System.Threading.Tasks;

class Program
{
    static void F1()           { Console.WriteLine("F1"); }
    static void F2(object obj) { Console.WriteLine("F2"); }
    static int  F3()           { Console.WriteLine("F3"); return 100; }
    static int  F4(object obj) { Console.WriteLine("F4"); Thread.Sleep(3000); return 200; }

    static void Main()
    {
        Task t1 = new Task(F1);
        t1.Start();

        Task t2 = new Task(F2, "Hello");
        t2.Start();

        Task<int> t3 = new Task<int>(F3);
        t3.Start();

        Task<int> t4 = new Task<int>(F4, "Hello");
        t4.Start();

        //t4.Wait(); // Join()

        Console.WriteLine($"{t4.Result}");
    }
}
```

```csharp
using System;
using System.Threading;
using System.Threading.Tasks;

class Program
{
    static void F1()           { Console.WriteLine("F1"); }
    static void F2(object obj) { Console.WriteLine("F1"); }
    static int  F3(object obj) { Console.WriteLine("F2"); return 100; }

    static void Main()
    {
        Task t1 = new Task(F1);         t1.Start();
        Task t2 = new Task(F2, "Hello");t2.Start();

        Task<int> t3 = new Task<int>(F3, "Hello"); t3.Start();


        Task t4 = Task.Run(F1);
        Task t5 = Task.Run(() => F2("Hello"));

        Task<int> t6 = Task.Run(() => F3("Hello"));


    }
}
```

```csharp
using System;
using System.Threading;
using System.Threading.Tasks;

class Program
{
    static void F1() 
    {
        // Task는 ThreadPool에 있는지 확인해보자.
        Console.WriteLine($"{Thread.CurrentThread.IsThreadPoolThread}");    // true

        // Background에 있기에 ThreadPool에 있는것으로 예상된다.
        Console.WriteLine($"{Thread.CurrentThread.IsBackground}");    // true
    }

    static void Main()
    {
        //Task t1 = new Task(F1); 
        Task t1 = new Task(F1, TaskCreationOptions.LongRunning);    
        // LongRunning을 줄경우 새로운 Thread를 생성하게 됨

        t1.Start();
        t1.Wait();

        Task t2 = Task.Run(F1);
    }
}
```

---

## Task 연속실행

```csharp
using System;
using System.Threading;
using System.Threading.Tasks;

class Program
{
    public static int Sum(int cnt)
    {
        int s = 0;
        for (int i = 0; i <= cnt; i++)
            s += i;
        return s;
    }

    public static void Main()
    {
        Task<int> t = Task.Run(() => Sum(1000));

        t.ContinueWith(메소드);

        // 아래 2줄은 주스레드가 Block 되게 된다.
        //t.Wait();
        //int n = t.Result;
    }
}
```

```csharp
using System;
using System.Threading;
using System.Threading.Tasks;

class Program
{  
    public static int Sum(int cnt)
    {
        int s = 0;
        for( int i = 0; i <= cnt; i++)
            s += i;
        Console.WriteLine("Finish Sum");
        return s;
    }
    public static void Main()
    {
        Task<int> t = Task.Run( () => Sum(1000) );

        t.ContinueWith(Foo);
        t.ContinueWith(Goo);
        t.ContinueWith((task) => Console.WriteLine("lambda"));

        Console.ReadLine();
    }

    public static void Foo(Task<int> t)
    {
        Console.WriteLine($"Foo : {t.Result}");
    }
    public static void Goo(Task<int> t)
    {
        Console.WriteLine($"Goo : {t.Result}");
    }
}
```

```csharp
using System;
using System.Threading;
using System.Threading.Tasks;

class Program
{
    public static int Sum(int cnt)
    {
        Console.WriteLine($"Sum : {Thread.CurrentThread.ManagedThreadId}");
        int s = 0;
        for (int i = 0; i <= cnt; i++)
            s += i;
        Console.WriteLine("Finish Sum");
        return s;
    }
    public static void Main()
    {
        Console.WriteLine($"Main : {Thread.CurrentThread.ManagedThreadId}");
        Task<int> t = Task.Run(() => Sum(1000));

        // 같은 Thread가 실행하게 하려면 옵션을 넣자
        t.ContinueWith(Foo, TaskContinuationOptions.ExecuteSynchronously) ;

        t.ContinueWith(Goo, TaskContinuationOptions.ExecuteSynchronously);

        //t.ContinueWith((task) => Console.WriteLine("lambda"));

        Console.ReadLine();  
    }

    public static void Foo(Task<int> t)
    {
        Console.WriteLine($"Foo : {Thread.CurrentThread.ManagedThreadId}");
        Console.WriteLine($"Foo : {t.Result}");
    }
    public static void Goo(Task<int> t)
    {
        Console.WriteLine($"Goo : {Thread.CurrentThread.ManagedThreadId}");
        Console.WriteLine($"Goo : {t.Result}");
    }
}
```

```csharp
using System;
using System.Threading;
using System.Threading.Tasks;

class Program
{
    public static int Sum(int cnt)
    {
  
        return 100;
    }
    public static void Main()
    {
        Task<int> t = Task.Run(() => Sum(1000));

        t.ContinueWith(F1, TaskContinuationOptions.NotOnCanceled);
        t.ContinueWith(F2, TaskContinuationOptions.OnlyOnFaulted);
        t.ContinueWith(F3, TaskContinuationOptions.OnlyOnRanToCompletion);

        Console.ReadLine();
    }
    public static void F1(Task<int> t) { Console.WriteLine($"F1 : {t.Result}");  }
    public static void F2(Task<int> t) { Console.WriteLine($"F2 : {t.Result}");  }
    public static void F3(Task<int> t) { Console.WriteLine($"F3 : {t.Result}"); }
}
```