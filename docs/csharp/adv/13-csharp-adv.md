---
layout: default
title: "13. Async IO"
parent: (C# Adv)
grand_parent: "(C#)"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## Async IO

* IO작업의 Async와 기존 Async작업의 차이가 있을까?
* IO작업은 HardWare에게 작업을 요청(입출력대기),
* Async는 Thread Pool에서 Thread를 생성(내가)해 작업.

```csharp
using System;
using System.IO;

class Program
{
   
    public static void WriteSync()
    {
        using (FileStream fs = new FileStream("a.txt", FileMode.Create))
        {
            byte[] buff = new byte[1024 * 1024 * 1000]; // 1G

            fs.Write(buff); //  동기적으로 쓰기

            Console.WriteLine("WriteSync Finish");
        }
    }

    public static async void WriteAsync()
    {
        using (FileStream fs = new FileStream("a.txt", FileMode.Create))
        {
            byte[] buff = new byte[1024 * 1024 * 1000]; // 1G

            await fs.WriteAsync(buff);

            Console.WriteLine("WriteAsync Finish");
        }
    }


    public static void Main()
    {
        //WriteSync();
        WriteAsync();
        Console.WriteLine("Main");
        Console.ReadLine();
    }
}
```