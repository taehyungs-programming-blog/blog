---
layout: default
title: "04. graph"
parent: "(자료구조 & 알고리즘)"
grand_parent: C#
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## Stack, Queue

* [Get Code 🌎](https://github.com/EasyCoding-7/Algorithm-Example/tree/07)

* Stack - LIFO(후입선출, Last in First out)
* Queue - FIFO(선입선출, First in First out)

```csharp
namespace Excercise
{
    class Program
    {
        static void Main(string[] args)
        {
            Stack<int> stack = new Stack<int>();
            Queue<int> queue = new Queue<int>();

            stack.Push(101);
            stack.Push(102);
            stack.Push(103);
            stack.Push(104);
            stack.Push(105);

            int data = stack.Peek();
            data = stack.Pop();

            queue.Enqueue(101);
            queue.Enqueue(102);
            queue.Enqueue(103);
            queue.Enqueue(104);
            queue.Enqueue(105);

            int data2 = queue.Peek(); 
            data2 = queue.Dequeue();
        }
    }
}
```

* **Q. 그냥 LinkedList쓰면 안되나?** - LIFO, FIFO를 강제한다. 또한 서로의 의사소통에도 좋다(Stack으로 구현할께요 안에 많은 의미가 포함된다.)

---

## 그래프란?

* 정점(Vertext) - 데이터를 표현
* 간선(Edge) - 정점들을 연결하는데 사용, 간선에 가중치/방향이 들어가기도 한다.

```csharp
// 정점간의 연결을 아래와 같이 표시하기도 한다.
List<int>[] adjacent = new List<int>[6];
{
    new List<int> { 1, 3 },
    new List<int> { 0, 2, 3 },
    new List<int> {  },
    new List<int> { 4 },
    new List<int> {  },
    new List<int> { 4 }
}
```

```csharp
// 만약 가중치도 필요하다면
class Edge
{
    public Edge(int v, int w) { vertex = v; weight w; }
    public int vertex;
    public int weight;
}

List<Edge>[] adjacent = new List<Edge>[6];
{
    new List<Edge>() { new Edge(1, 15), new Edge(3, 35) },
    // ...
}
```

* List를 사용하기에 임의접근이 어렵다?

```csharp
// 2차원 배열로 저장해본다면?
int[,] adjacent2 = new int[6, 6]
{
    {0, 1, 0, 1, 0, 0},
    {1, 0, 1, 1, 0, 0},
    {0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 1, 0},
    {0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 1, 0},
}

adjacent2[1][3];    // 1 -> 3으로 가는길 찾기
```

```csharp
// 가중치 표현도 가능하다
int[,] adjacent2 = new int[6, 6]
{
    {-1, 15, -1, 35, -1, -1},
    {15, -1, 5, 10, -1, -1},
    {-1, -1, -1, -1, -1, -1},
    // ...
}
```
