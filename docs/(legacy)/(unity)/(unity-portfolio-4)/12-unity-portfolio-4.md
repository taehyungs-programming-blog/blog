---
layout: default
title: "12. A* Algorithm"
parent: "(포트폴리오 4 - 방치형 게임)"
grand_parent: "(Unity ✨)"
great_grand_parent: "Legacy Archive"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌎](https://github.com/Arthur880708/Unity.IncrementalGame.Example/tree/4)

---

# A* Algorithm

```csharp
public List<Vector3Int> FindPath(Vector3Int startCellPos, Vector3Int destCellPos, int maxDepth = 10)
{
    Dictionary<Vector3Int, int> best = new Dictionary<Vector3Int, int>();
    Dictionary<Vector3Int, Vector3Int> parent = new Dictionary<Vector3Int, Vector3Int>();

    // PriorityQueue는 그리 어렵지 않아 설명생략 
        // 그냥 compareto를 보고 내부에서 list정렬
    PriorityQueue<PQNode> pq = new PriorityQueue<PQNode>();

    Vector3Int pos = startCellPos;
    Vector3Int dest = destCellPos;

    Vector3Int closestCellPos = startCellPos;
    int closestH = (dest - pos).sqrMagnitude;

    // 우선 시작점을 pq에 넣어둔다
    {
        int h = (dest - pos).sqrMagnitude;
        pq.Push(new PQNode() { H = h, CellPos = pos, Depth = 1 });
        parent[pos] = pos;
        best[pos] = h;
    }

    // 이제 pq를 순회하며 최적의 경로를 찾기 시작한다
    while (pq.Count > 0)
    {
        PQNode node = pq.Pop();
        pos = node.CellPos;

        // 목적지 도착
        if (pos == dest)
            break;

        // 무한 순회를 방지하자
        if (node.Depth >= maxDepth)
            break;

        // 갈수있는 방향을 체크
        foreach (Vector3Int delta in _delta)
        {
            Vector3Int next = pos + delta;

            if (CanGo(next) == false)
                continue;

            int h = (dest - next).sqrMagnitude;

            if (best.ContainsKey(next) == false)
                best[next] = int.MaxValue;

            if (best[next] <= h)
                // 더 최적의 경로가 있었다면 pass ...
                continue;

            best[next] = h;

            pq.Push(new PQNode() { H = h, CellPos = next, Depth = node.Depth + 1 });
            // Parent는 next는 pos에서 왔습니다!를 알려준다
            parent[next] = pos;

            // 목적지까지는 못 가더라도, 그나마 제일 좋았던 후보 기억.
            if (closestH > h)
            {
                closestH = h;
                closestCellPos = next;
            }
        }
    }

    // 제일 가까운 애라도 찾음.
    if (parent.ContainsKey(dest) == false)
        return CalcCellPathFromParent(parent, closestCellPos);

    return CalcCellPathFromParent(parent, dest);
}
```

