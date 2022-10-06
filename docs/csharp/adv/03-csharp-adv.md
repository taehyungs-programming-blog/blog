---
layout: default
title: "3. 값(Struct) 타입 생성자"
parent: (C# Adv)
grand_parent: C#
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## 값 타입(Struct) 생성자

### 참조 Vs 값 타입 생성자

```csharp
using System;

class CPoint
{
	public int x;
	public int y;

	public CPoint(int a, int b){ x = a; y = b; }
}

struct SPoint
{
	public int x;
	public int y;
	public SPoint() {}	// Error
	public SPoint(int a, int b){ x = a; y = b; }
}

class Program
{
	static void Main(string[] args)
	{
		CPoint cp1 = new CPoint(1, 2);
		CPoint cp2 = new CPoint();		// Error - 생성자가 없음
		// Reference Type - 생성자를 구현하지 않는 경우, 컴파일러가 인자없는 생성자 제공 O
			// 단, 생성자를 구현할 경우 컴파일러는 생성자를 제공하지 않음.
		SPoint sp1 = new SPoint(1, 2);
		SPoint sp2 = new SPoint();		// Okay - 
		// Value Type - 사용자가 인자없는 생성자를 만들 수 없다.
			// 단, CLR에서 생성자 없이 생성이 가능하게 제공
	}
}
```

```csharp
using System;

struct CPoint
{
	public int x = 0;	// Error
	// 값 타입의 경우 필드초기화가 불가능.
	// 이미 하나의 객체이다.
	public int y;

}
```

```csharp
public static void Main()
{
	CPoint cp1;		// 객체의 생성이 아님, 참조 변수의 생성이다.
	CPoint cp2 = new CPoint();	// 객체생성

	SPoint sp1;		// 객체의 생성, 단 메모리만 할당
	SPoint sp2 = new SPoint();	// 객체의 생성, initobj로 초기화


	Console.WriteLine($"{sp1.x}");	// Error - 초기화가 되지않은 변수를 읽을수 없다.
	Console.WriteLine($"{sp2.x}");
}
```

```csharp
struct SPoint
{
	public int x;
	public int y;
}
class CCircle
{
	public SPoint center;
}
struct SCircle
{
	public SPoint center;
}

class Program
{
	public static void Main()
	{
		CCircle cc1;			// 객체 아님 참조 변수
		CCircle cc2 = new CCircle();	// 객체 생성, 모든 멤버가 0초기화
		SCircle sc1;			// 객체 생성 단, 초기화 안됨
		SCircle sc2 = new SCircle();	// 객체 생성 후 초기화 됨.
	}
}
```

```csharp
class SPoint	// class임 주목!
{
	public int x;
	public int y;
	public int cnt;	// 생성자에서 굳이 초기화를 안해도 자동 0으로 초기화

	public SPoint(int a, int b) { x = a; y = b; }
}

class Program
{
	public static void Main()
	{
		SPoint pt = new SPoint(1, 2);
	}
}
```

```csharp
struct SPoint
{
	public int x;
	public int y;
	public int cnt;	

	public SPoint(int a, int b) 
	{ 
		x = a; 
		y = b; 
		// Error - cnt를 초기화 하지 않았기에 에러
		// struct는 new없이 생성이 가능하기에 발생한 문제
	}
}
```
