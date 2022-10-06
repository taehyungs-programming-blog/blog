---
layout: default
title: "8. C# 8.0 최신 문법"
parent: (C# Adv)
grand_parent: C#
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## C# 8.0 최신문법

### overroad

```csharp
using System;

interface ICamera
{
    // 자녀에서 꼭 takePicture()를 구현해야한다.
    void takePicture();

    // 자녀에서 꼭 uploadSNS()를 구현하지 않아도 됨(부모에서 이미구현)
    void uploadSNS()
    {
        Console.WriteLine("upload SNS");
    }
}

class Camera : ICamera
{
    public void takePicture()
    {
        Console.WriteLine("Take Picture With Camera");
    }
    public void uploadSNS()
    {
        Console.WriteLine("Camera upload SNS");
    }
}

class Program
{
    static void Main()
    {
        Camera c = new Camera();
        c.takePicture();
        c.uploadSNS();

        ICamera ic = c;
        ic.uploadSNS();
    }
}
```

---

### using

```csharp
// using
using System;
using System.IO;
using static System.Console; // WriteLine("AA")

class Program
{
    static void Main()
    {
        FileStream f1 = new FileStream("a1.txt", FileMode.CreateNew);
        f1.Dispose();

        using (FileStream f2 = new FileStream("a2.txt", FileMode.CreateNew))
        {

        } // 자동 f2.Dispose
    }

    public static void Foo()
    {
        //+ C# 8.0
        using FileStream f3 = new FileStream("a3.txt", FileMode.CreateNew);
        // {} 도 안넣어도 된다.
    } // f3.Dispose()
}
```

---

### nullable reference

```csharp
// nullable reference
using System;

class Program
{
    static void Main()
    {
        //int  n1 = null; // error
        //int? n2 = null;  // ok


#nullable enable       // 참조 타입 변수를 null 을 대입하면 경고..
        string s1 = null; // ok
        string? s2 = null;
#nullable disable 
        //int n = s1.Length;
    }
}
```

```csharp
using System;

class Program
{
    public static void Main()
    {
        string s1 = null;

        //# C# 6.0 NULL 병합 연산자
        string s2 = s1 ?? "hello";


        //# C# 8.0 NULL 병합 대입
        s1 = "hello";

        s1 ??= "world";  // if ( s1 == null ) s1 = "world"

        Console.WriteLine(s1);

    }
}
```