---
layout: default
title: "7. Local Function"
parent: (C# Adv)
grand_parent: "(C#)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## Local Function

```csharp
using System;

class Program
{
    public static void Foo()
    {
        int n = square(3);

        // Local Function
        int square(int a)
        {
            return a * a;
        }
    }

    /*
    public static double div(double a, double b)
    {
        // 실제 구현부와 Exception처리하는 부분과 섞여있다.

        if (b == 0)
            throw new Exception("divide by zero");

        return a / b;
    }
    */
    /*
    public static double div(double a, double b)
    { 
        return a / b;
    }
    */
    public static double div_wrapper(double a, double b)
    {
        if (b == 0)
            throw new Exception("divide by zero");

        return div(a, b);

        double div(double a, double b)
        {
            return a / b;
        }
    }

    public static void Main()
    {
        // 또한 Wrapper를 쓰면 장점이 실수로 div를 직접호출할수 없게 해준다.
        double ret = div_wrapper(10, 0);
        Console.WriteLine(ret);
    }
}
```

```csharp
// 위는 실제로는 그렇게 사용되진 않고 아래가 주로 사용됨.

using System;
using System.Collections;

// 1 ~ 5 까지의 숫자를 보관하는 컬렉션
class NumCollections : IEnumerable
{
    private int[] arr = new int[5] { 1, 2, 3, 4, 5 };

    public IEnumerator GetEnumerator()
    {
        // 오류만 확인..
        Console.WriteLine("arr 의 유효성 확인");
        if (arr == null) throw new Exception("null");

        return implementation();

        IEnumerator implementation()
        {
            foreach (int n in arr)
            {
                yield return n;
            }
        }
    }
}
class Program
{
    public static void Main()
    {
        NumCollections nums = new NumCollections();

        IEnumerator it = nums.GetEnumerator();
        Console.WriteLine("After GetEnumerator");

        while( it.MoveNext() )
        {
            Console.WriteLine(it.Current);
        }
    }
}
```

```csharp 
using System; 
class Program 
{ 
    public static int Foo(int a, int b) 
    { 
        int value = 10; 
        return goo(10); 
        static int goo(int n) 
        { 
	// Error - 지역변수에 접근불가 
            return value + a + b + n; 
        } 
    } 
    public static void Main() 
    { 
        Console.WriteLine(Foo(1, 2)); 
    } 
} 
```
