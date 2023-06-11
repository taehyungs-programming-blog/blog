---
layout: default
title: "12. Async"
parent: (C# Adv)
grand_parent: "(C#)"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## Async

```csharp
using System;
using System.Threading;
using System.Threading.Tasks;

class Program
{
    public static int Sum(int first, int last)
    {
        int s = 0;
        for (int i = first; i <= last; i++)
        {
            s += i;
            Thread.Sleep(10);
        }
        return s;
    }

    public static Task<int> SumAsync(int first, int last)
    {
        Task<int> t = Task.Run(() =>
        {
            int s = 0;
            for (int i = first; i <= last; i++)
            {
                s += i;
                Thread.Sleep(10);
            }
            return s;
        });
        return t;
    }

    public static void Main()
    {
        // Thread를 만들어서 처리하면 비동기가 된다.
        Task<int> ret = SumAsync(1, 200);

        Console.WriteLine("Main");
        Console.WriteLine($"{ret.Result}");


        //int ret = Sum(1, 200); // Blocking
        //Task<int> t = Task.Run(() => Sum(1, 200));
        
        //Console.WriteLine("Main");

        //Console.WriteLine($"{t.Result}");
    }
}
```

```csharp
using System;
using System.Threading;
using System.Threading.Tasks;

class Program
{
    public static Task<int> SumAsync(int first, int last)
    {
        return Task.Run(() =>
        {
            int s = 0;
            for (int i = first; i <= last; i++)
            {
                s += i;
                Thread.Sleep(10);
            }
            return s;
        });
    }

    public static void UpdateResult()
    {

        Task<int> t = SumAsync(1, 200);

        // 결과를 이런식으로 린턴받으면 결국 Block걸리게 된다.
            // 이걸 개선하고자 한다.
        //Console.WriteLine($"{t.Result}");

        var awaiter = t.GetAwaiter();

        awaiter.OnCompleted( () =>
            Console.WriteLine($"{awaiter.GetResult()}"));
   }

    public static void Main()
    {
        UpdateResult();

        Console.WriteLine("Main : Run Event Loop");
        Console.ReadLine();
    }
}

// 이걸 좀 더 편하게 MS에서 제공해준다.
```

```csharp
using System;
using System.Threading;
using System.Threading.Tasks;

class Program
{
    public static Task<int> SumAsync(int first, int last)
    {
        return Task.Run(() =>
        {
            int s = 0;
            for (int i = first; i <= last; i++)
            {
                s += i;
                Thread.Sleep(10);
            }
            return s;
        });
    }


    public static async void UpdateResult()
    {
        Console.WriteLine("UpdateResult");

        int ret = await SumAsync(1, 200); // 비동기 함수를 동기 함수 처럼 사용
        // await 까지 실행되고 Main으로 반환
            // await의 결과가 도착시 아래로 들어간다.

        Console.WriteLine($"{ret}");
    }

    public static void Main()
    {
        UpdateResult();

        Console.WriteLine("Main : Run Event Loop");
        Console.ReadLine();
    }
}
```

```csharp
    // ...

    //public static async void UpdateResult()
    //public static async Task UpdateResult()
    public static async Task<int> UpdateResult()
    {
        int ret = await SumAsync(1, 200); 

        Console.WriteLine($"{ret}");
        return ret;
    }


    public static void Main()
    {
        Task<int> t = UpdateResult();

        // UpdateResult()를 대기할수 있게 된다.
        //t.Wait();
        Console.WriteLine($"Main : {t.Result}");


        Console.WriteLine("Main : Run Event Loop");
        Console.ReadLine();
    }
}
```

```csharp
using System;
using System.Threading;
using System.Threading.Tasks;

class Program
{
    public static Task<int> SumAsync(int first, int last)
    {
        return Task.Run(() =>
        {
            int s = 0;
            for (int i = first; i <= last; i++)
            {
                s += i;
                Thread.Sleep( 10 );
            }
            return s;
        });
    }
    public static void Foo()
    {
        int first = i * 100 + 1;
        int last  = (i + 1) * 100;

        Task<int> t = SumAsync(first, last );

        var awaiter = t.GetAwaiter();
        awaiter.OnCompleted( () => Console.WriteLine($"{first} ~ {last} : {awaiter.GetResult()}"));
    }
    public static void Main()
    {
        Foo();

        Console.ReadLine();
    }
}
```