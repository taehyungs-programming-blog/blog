---
layout: default
title: "06. dijkstra algorithm"
parent: "(자료구조 & 알고리즘)"
grand_parent: "(C#)"
great_grand_parent: "Legacy Archive"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [참고사이트 🌎](https://blog.tomclansys.com/63)

```csharp
int[,] adj = new int[6, 6]
{
    { -1, 1, -1, 15, -1, -1 },
    { 1, -1, 1, 1, -1, -1 },
    { -1, 1, -1, -1, -1, -1 },
    { 35, 10, -1, -1, 05, -1 },
    { -1, -1, -1, 05, -1, 05 },
    { -1, -1, -1, -1, 05, -1 }
};

public void Dijikstra(int start)
{
    bool[] visited = new bool[6];
    int[] distance = new int[6];
    int[] parent = new int[6];
    Array.Fill(distance, Int32.MaxValue);
    distance[start] = 0;
    parent[start] = start;

    while(true)
    {
        int closest = Int32.MaxValue;
        int now = -1;

        // 가장 가까운 후보를 찾는다
        for(int i = 0; i < 6; i++)
        {
            if(visited[i]) 
                continue;
            if(distance[i] == Int32.MaxValue || distance[i] >= closest) 
                continue;
            
            closest = distance[i];
            now = i;
        }

        if(now == -1)
            break;

        visited[now] = true;

        // 방문한 정점과 인접한 정점 확인
        for(int next = 0; next < 6 ; next++)
        {
            if(adj[now, next] == -1)
                continue;
            if(visited[next])
                continue;

            int nextDist = distance[now] + adj[now, next];
            if(nextDist < distance[next])
            {
                distance[next] = nextDist;
                parent[next] = now;
            }
        }
    }
}
```

