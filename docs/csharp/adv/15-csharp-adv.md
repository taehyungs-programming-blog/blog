---
layout: default
title: "15. Stream Adapter"
parent: (C# Adv)
grand_parent: "(C#)"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## Adapter Pattern

```csharp
using System;
using System.Collections.Generic;

class Stack 
{
    public List<int> st = null; 
    public Stack(List<int> s) { st = s; }
    
    public void Push(int a) { st.Add(a);  }
    public int  Pop()
    {
        int n = st.Count - 1;
        int temp = st[n];
        st.RemoveAt(n);
        return temp;
    }
}

class Program
{
    public static void Main()
    {
        List<int> st = new List<int>();

        st.Add(10);
        st.Add(20);

        // st를 stack 처럼 사용하고 싶다.
        Stack s = new Stack(st);

        s.Push(30);
        s.Push(40);

        int n = s.Pop(); // 40
        Console.WriteLine(n);

        
    }
}
```

---

## Stream Adapter

```csharp
using System;
using System.IO;
using System.IO.Compression;
using System.Text;

class Program
{
    public static void Main()
    {
        Console.WriteLine("Hello");

        FileStream fs = new FileStream("D:\\a.txt", FileMode.Create);

        // "Hello" 를 파일에 쓰고 싶다.
        //string s = "hello";
        //byte[] buff = Encoding.ASCII.GetBytes(s);

        //fs.Write(buff, 0, buff.Length);

        StreamWriter sw = new StreamWriter(fs, Encoding.ASCII);
        sw.WriteLine("Hello");

       // GZipStream gz = new GZipStream(fs);
       // gz.Write()

        sw.Close();
        fs.Close();
    }
}
```