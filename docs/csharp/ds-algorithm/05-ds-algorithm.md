---
layout: default
title: "05. dfs, bfs"
parent: "(자료구조 & 알고리즘)"
grand_parent: "(C#)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## 그래프 순회 방법

* DFS(깊이 우선 탐색)
* BFS(너비 우선 탐색)

---

## DFS

* [Get Code 🌎](https://github.com/EasyCoding-7/Algorithm-Example/tree/08)

```csharp
class Graph
{
    int[,] adj = new int[6, 6]
    {
        { 0, 1, 0, 1, 0, 0 },
        { 1, 0, 1, 1, 0, 0 },
        { 0, 1, 0, 0, 0, 0 },
        { 1, 1, 0, 0, 1, 0 },
        { 0, 0, 0, 1, 0, 0 },
        { 0, 0, 0, 0, 1, 0 }
    };

    List<int>[] adj2 = new List<int>[]
    {
        new List<int>() {1, 3},
        new List<int>() {0, 2, 3},
        new List<int>() {1},
        new List<int>() {0, 1, 4},
        new List<int>() {3, 5},
        new List<int>() {4}
    };

    bool[] visited = new bool[6];

    public void DFS(int now)
    {
        // now 방문이후 깊이우선으로 now와 연결된 정점을 방문
        Console.WriteLine(now);
        visited[now] = true;

        for(int next = 0; next < 6; next++)
        {
            if (adj[now, next] == 0)
                continue;
            if (visited[next])
                continue;

            DFS(next);
        }
    }
}
```

---

## BFS

* [Get Code 🌎](https://github.com/EasyCoding-7/Algorithm-Example/tree/09)

* BFS는 주로 길찾기 알고리즘에 사용된다.
* 시작위치에서 몇 번째 떨어져 있는지 구할수 있기 때문이다.

```csharp
public void BFS(int start)
{
    bool[] found = new bool[6];
    Queue<int> q = new Queue<int>();
    q.Enqueue(start);
    found[start] = true;

    while(q.Count > 0)
    {
        int now = q.Dequeue();
        Console.WriteLine(now);

        for(int next = 0; next< 6; next++)
        {
            if (adj[now, next] == 0)
                continue;
            if (found[next])
                continue;

            q.Enqueue(next);
            found[next] = true;
        }
    }
}
```

```csharp
// 거리를 구해보자.
public void BFS(int start)
{
    bool[] found = new bool[6];
    int[] parent = new int[6];
    int[] distance = new int[6];

    Queue<int> q = new Queue<int>();
    q.Enqueue(start);
    found[start] = true;
    parent[start] = start;
    distance[start] = 0;

    while(q.Count > 0)
    {
        int now = q.Dequeue();
        Console.WriteLine(now);

        for(int next = 0; next< 6; next++)
        {
            if (adj[now, next] == 0)
                continue;
            if (found[next])
                continue;

            q.Enqueue(next);
            found[next] = true;
            parent[next] = now;                     // 어디서 부터 왔는지 구할수 있음.
            distance[next] = distance[now] + 1;     // 이동거리 구할수 있음.

            // 이런면에서 길찾기 알고리즘에 적합하다
        }
    }
}
```

---

## 길찾기에 BFS 적용

* [Get Code 🌎](https://github.com/EasyCoding-7/Algorithm-Example/tree/10)

```csharp
 public void BFS()
{
    int[] deltaY = new int[] { -1, 0, 1, 0 };
    int[] deltaX = new int[] { 0, -1, 0, 1 };
    bool[,] found = new bool[_board.Size, _board.Size];
    Pos[,] parent = new Pos[_board.Size, _board.Size];

    Queue<Pos> q = new Queue<Pos>();
    q.Enqueue(new Pos(PosY, PosX));
    found[PosY, PosX] = true;
    parent[PosY, PosX] = new Pos(PosY, PosX);


    while (q.Count > 0)
    {
        Pos pos = q.Dequeue();
        int nowY = pos.PosY;
        int nowX = pos.PosX;

        for(int i = 0; i < 4; i++)
        {
            int nextY = nowY + deltaY[i];
            int nextX = nowX + deltaX[i];

            if(nextX < 0 || nextY < 0 || nextX > _board.Size || nextY > _board.Size) continue;
            if (_board.Tile[nextY, nextX] == Board.TileType.Wall) continue;
            if (found[nextY, nextX]) continue;

            q.Enqueue(new Pos(nextY, nextX));
            found[nextY, nextX] = true;
            parent[nextY, nextX] = new Pos(nowY, nowX);
        }
    }

    int y = _board.DestY;
    int x = _board.DestX;
    while (parent[y, x].PosY != y || parent[y, x].PosX != x)
    {
        _points.Add(new Pos(y, x));
        Pos pos = parent[y, x];
        y = pos.PosY;
        x = pos.PosX;
    }
    _points.Add(new Pos(y, x));
    _points.Reverse();
}
```
