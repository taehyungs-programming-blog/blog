---
layout: default
title: "09. A* algorithm"
parent: "(자료구조 & 알고리즘)"
grand_parent: C#
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌎](https://github.com/EasyCoding-7/Algorithm-Example/tree/11)

* 기존 BFS의 문제는 목적지 까지 일직선이라도 노드를 지날때 마다 매번 앞/뒤/옆을 모두 체크한다는 문제가 있었다.
* A* 로 해결해 보자.
    * 대략적 설명은 현재지점에서 목적지까지 최단거리를 계산한다에 있음(자세한건 코드로 보자)

```csharp
struct PQNode : IComparable<PQNode>
{
    public int F;
    public int G;
    public int Y;
    public int X;

    public int CompareTo(PQNode other)
    {
        if (F == other.F)
            return 0;
        return F < other.F ? 1 : -1;
    }
}

public void AStar()
{
    // 점수
    // F = G + H
    // F = 최종 점수(작을수록 가까움을 표현)
    // G = 시작점에서 해당 좌표까지 이동하는데 드는 비용(작을 수록 좋음)
    // H = 목적지에서 얼마나 가까운지 (역시 작을수록 좋음)

    // (y, x)에 이미 방문했는지 체크
    bool[,] closed = new bool[_board.Size, _board.Size];

    // (y, x)가는 길을 한 번이라도 발견했는지 체크
    int[,] open = new int[_board.Size, _board.Size];
    for (int y = 0; y < _board.Size; y++)
        for (int x = 0; x < _board.Size; x++)
            open[y, x] = Int32.MaxValue;

    Pos[,] parent = new Pos[_board.Size, _board.Size];

    // 시작점
    open[PosY, PosX] = 0/*G*/ + Math.Abs(_board.DestY - PosY) + Math.Abs(_board.DestX - PosX)/*H*/;

    // 오픈리스트에 있는 정보를 빠르게 뽑아오기 위한 도구
    PriortyQueue<PQNode> pq = new PriortyQueue<PQNode>();
    pq.Push(new PQNode() { F= Math.Abs(_board.DestY - PosY) + Math.Abs(_board.DestX - PosX), G=  0, Y = PosY, X = PosX });
    parent[PosY, PosY] = new Pos(PosY, PosX);

    int[] deltaY = new int[] { -1, 0, 1, 0 };
    int[] deltaX = new int[] { 0, -1, 0, 1 };
    int[] cost = new int[] { 1, 1, 1, 1 };

    while (pq.Count > 0)
    {
        PQNode node = pq.Pop();
        
        // 방문여부 체크
        if (closed[node.Y, node.X]) continue;
        closed[node.Y, node.X] = true;

        // 목적지 도착
        if (node.X == _board.DestX && node.Y == _board.DestY) break;

        // 이동가능여부 체크
        for(int i =0; i < deltaY.Length; i++)
        {
            int nextY = node.Y + deltaY[i];
            int nextX = node.X + deltaX[i];

            if (nextX < 0 || nextY < 0 || nextX > _board.Size || nextY > _board.Size) continue;
            if (_board.Tile[nextY, nextX] == Board.TileType.Wall) continue;
            if (closed[nextY, nextX]) continue;

            // 비용계산
            int g = node.G + cost[i];
            int h = Math.Abs(_board.DestY - nextY) + Math.Abs(_board.DestX - nextX);
            // 다른길이 더 빠르다면 스킵
            if (open[nextY, nextX] < g + h) continue;

            open[nextY, nextX] = g + h;
            pq.Push(new PQNode() { F = g + h, G = g, Y = nextY, X = nextX });
            parent[nextY, nextX] = new Pos(node.Y, node.X);
        }
    }

    CalcPathFromParent(parent);
}
```


