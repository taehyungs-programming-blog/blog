---
layout: default
title: "2. 참조(Class) 타입 생성자"
parent: (C# Adv)
grand_parent: C#
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## 생성자

```csharp
using System;

public class Point
{
	public int x;
	public int y;

	public Point(int x, int y)
	{
		Console.WriteLine("Point Constructor");
		this.x = x;
		this.y = y;
	}
}

class Program
{
	static void Main(string[] args)
	{
		Point pt = new Point(1, 2);
		Console.WriteLine($"{pt.x} {pt.y}");
	}
}
```

* 생성자는 네 가지 버전이 존재한다.
	* 참조 타입(Class) 생성자
	* 값 타입(Struct) 생성자
	* Static 생성자
	* Deconstructor(C# 7.0 ~)

---

## 참조 타입(Class) 생성자

```csharp
using System;

// class의 생성자를 참조타입 생성자라 한다.
public class Point
{
	public int x;
	public int y;

	public Point(int x, int y)
	{
	}
}

class Program
{
	static void Main(string[] args)
	{
		Point pt = new Point(1, 2);
		Console.WriteLine($"{pt.x} {pt.y}");	
		// 0 0 
		// 참조타입은 메모리가 먼저 0으로 초기화 된다.
	}
}
```

### 상속과 생성자

```csharp
using System;
using static System.Console;

class Base
{
	public Base() { WriteLine("Base()"); }
	public Base(int n) { WriteLine("Base(int)"); }
}
class Derived : Base
{
	public Derived() { WriteLine("Derived()"); }
	public Derived(int n) { WriteLine("Derived(int)"); }
}

class Program
{
	public static void Main()
	{
		Derived d = new Derived();
		// Base()
		// Derived()

		Derived d1 = new Derived(1);
		// Base()
		// Derived(int)
		/*
		* 좀 더 적확히는
		public Derived(int) : base() 이게 호출됨
		*/
	}
}
```

```csharp
class Derived : Base
{
	public Derived() { WriteLine("Derived()"); }
	public Derived(int n) : base(n)	{}
	// 이렇게 선언시 base에 n을 넘겨준다
```

```csharp
// Quize
class Animal
{
	protected Animal() {}
}
class Dog : Animal
{
	public Dog() {}
}

class Program
{
	static void Main()
	{
		// 에러가 어디서 발생할까?
		Animal a = new Animal();
		Dog d= new Dog();
	}
}
```

* `Animal`에서 에러발생 -> 직접 생성을 못하게 할시 사용된다.

### 생성자와 가상함수

```csharp
using System;
using static System.Console;

class Base
{
	public Base() { Foo(); }
	public virtual void Foo() { WriteLine("Base.Foo()"); }
}
class Derived : Base
{
	public int a = 100;
	public int b;

	public Derived() { b = 100; }
	public override void Foo() { WriteLine($"Derived.Foo : {a}, {b}"); }
}

class Program
{
	public static void Main()
	{
		Derived d = new Derived();
		// Derived.Foo : 100, 0
		// b는 아직 초기화가 안됨.
		// Foo호출 이후 초기화 된다.
	}
}
```
