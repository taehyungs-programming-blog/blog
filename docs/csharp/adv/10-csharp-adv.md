---
layout: default
title: "10. Thread Pool"
parent: (C# Adv)
grand_parent: C#
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## Thread Pool

* Thread Pool의 필요성
    * Thread를 생성과 파괴에는 오버헤드가 발생한다.
    * 사용자 CPU에서 최적화된 Thread개수를 찾기 쉽지않다.
* Thread는 생성/파괴보다는 재활용이 핵심이다.
* CLR에게 현재 시스템환경을 고려하여 최적의 스레드를 관리.

---

## 사용

```csharp
using System;
using System.Threading;

public static class Program
{
    private static void Foo(object arg)
    {
        Console.WriteLine($"Foo : {arg}, {Thread.CurrentThread.ManagedThreadId}");
        Thread.Sleep(1000);

        Console.WriteLine($"{Thread.CurrentThread.IsThreadPoolThread}");

        Console.WriteLine("Finish Foo");

        Console.ReadLine();
    }

    public static void Main()
    {
        //ThreadPool.QueueUserWorkItem(Foo, "Hello");
        ThreadPool.QueueUserWorkItem(Foo); // arg 에 null 

        // * 참고 Thread Pool의 Thread는 항상 Background Thread이다.

        Console.ReadLine();
    }
}
```