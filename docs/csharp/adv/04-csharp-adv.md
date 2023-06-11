---
layout: default
title: "4. static 생성자, Deconstructor"
parent: (C# Adv)
grand_parent: "(C#)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## static 생성자

```csharp
class Point
{
	public int x;
	public int y;
	public static int cnt;
	public Point(int a, int b)
	{
		x = a;
		y = b;

		// pt1에서 한 번 초기화 됐는데 또 초기화 해야할까?
			// -> static 생성자로 해결해 보자.
		cnt = 0;
	}
}

class Program
{
	public static void Main()
	{
		Point pt1 = new Point(1, 1);
		Point pt2 = new Point(2, 2);
	}
}
```

```csharp
class Point
{
	public int x;
	public int y;
	public static int cnt;
	public Point(int a, int b)
	{
		x = a;
		y = b;
	}
	// static 생성자 - 딱 한 번만 호출된다.
	static Point()
	{
		cnt = 0;
	}
}
```

```csharp
class Point
{
	public int x;
	public int y;
	public static int cnt;

	public Point(int a, int b) { Console.WriteLine("instance ctor"); }
	static Point()
	{
		cnt = 0;
		Console.WriteLine("static ctor");
	}
}

class Program
{
	public static void Main()
	{
		Point pt1 = new Point(1, 1);
		/*
		static ctor
		instance ctor
		*/

		Point pt2 = new Point(1, 1);
		/*
		instance ctor
		*/
	}
}
```

---

## Deconstructor

```csharp
class Point
{
	public int x;
	public int y;
	public Point(int a, int b) { x = a; y = b; }
}

class Program
{
	public static void Main()
	{
		Point pt1 = new Point(1, 1);
		// 이걸 좀더 쉽게 쓰고싶다
		pt1.x;
		pt1.y;
	}
}
```

```csharp
class Point
{
	public int x;
	public int y;
	public Point(int a, int b) { x = a; y = b; }
	public void Deconstruct(out int a, out int b) 
	{
		a = x;
		b = y;
	}
}

class Program
{
	public static void Main()
	{
		Point pt1 = new Point(1, 1);
		var (x, y) = pt;
	}
}
```