---
layout: default
title: "5. Index & Range"
parent: (C# Adv)
grand_parent: C#
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## Index & Range

### Index

```csharp
using System;

class Program
{
	static void Main()
	{
		int[] arr = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

		int n1 = arr[2];		// 3
		int n2 = arr[^2];		// 9
	}
}
```

```csharp
using System;

class Program
{
	static void Main()
	{
		string s = "ABCCDEFGHI";

		int idx1 = 2;
		char c1 = s[idx1];		// C

		Index idx2 = new Index(2);
		c1 = s[idx2];			// C

		Index idx3 = new Index(2, fromEnd: true);
		c1 = s[idx3];			// H
	}
}
```

```csharp
using System;

class Program
{
	static void Main()
	{
		string s = "ABCCDEFGHI";

		Index i1 = new Index(3);
		Index i2 = new Index(3, fromEnd: true);

		Index i3 = Index.FromStart(3);
		Index i4 = Index.FromEnd(3);

		Index i5 = 3;
		Index i6 = ^3;
	}
}
```

```csharp
// 사용자 정의타입
class Sentense
{
	private string[] words = null;
	public Sentense(string s) { words = s.Split(); }
	// indexer
	public string this[int idx] { get { return words[idx]; } }
	public string this[Index idx] { get { return words[idx]; } }
}
// 좀 더 쉽게 구현해보자면?
```

```csharp
class Sentense
{
	private string[] words = null;
	public Sentense(string s) { words = s.Split(); }
	// indexer
	public string this[int idx] { get { return words[idx]; } }
		// Length나 Count를 제공하면 된다.
	public int Length { get { return words.Length; }}
	public int Count { get { return words.Length; }}
}
```

---

### Range

```csharp
using System;

class Program
{
	static void Main()
	{
		string s1 = "ABCDEFGHI";

		char c = s1[2];

		string s2 = s1[2..7];	// CDEFG
		string s3 = s1[2..^3];	// CDEFG
	}
}
```

```csharp
using System;

class Program
{
	static void Main()
	{
		int[] arr = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

		Range r1 = new Range(new Index(2), new Index(2, true));
		Range r2 = new Range(2, ^2);	// 상동
		Range r3 = 2..^2;
		int[] arr2 = arr1[r1];

		foreach(var n in arr2)
			Console.WriteLine(n);
	}
}
```

```csharp
using System;

class Program
{
	static void Main()
	{
		string s1 = "ABCDEFGHI";

		Range r1 = new Range();
		Range r2 = new Range(2, ^2);

		Range r3 = Range.All;
		Range r4 = Range.StartAt(4);
		Range r5 = Range.EndAt(4);

		Range r6 = 2..7;
	}
}
```