---
layout: default
title: "20. 대형몬스터 구현"
parent: "(포트폴리오 4 - 방치형 게임)"
grand_parent: "(Unity ✨)"
great_grand_parent: "Legacy Archive"
nav_order: 3
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌎](https://github.com/Arthur880708/Unity.IncrementalGame.Example/tree/8)

---

## 타일을 2칸 이상 차지하는 경우 대형몬스터라 간주

* 타 2칸이상 체크하는 로직이 필요해진다.

```csharp
public bool CanGo(BaseObject self, Vector3Int cellPos, bool ignoreObjects = false, bool ignoreSemiWall = false)
{
    int extraCells = 0;
    if (self != null)
        extraCells = self.ExtraCells;

    // extraCells을 한 번 더 체크
    for (int dx = -extraCells; dx <= extraCells; dx++)
    {
        for (int dy = -extraCells; dy <= extraCells; dy++)
        {
            Vector3Int checkPos = new Vector3Int(cellPos.x + dx, cellPos.y + dy);

            if (CanGo_Internal(self, checkPos, ignoreObjects, ignoreSemiWall) == false)
                return false;
        }
    }

    return true;
}

bool CanGo_Internal(BaseObject self, Vector3Int cellPos, bool ignoreObjects = false, bool ignoreSemiWall = false)
{
    if (cellPos.x < MinX || cellPos.x > MaxX)
        return false;
    if (cellPos.y < MinY || cellPos.y > MaxY)
        return false;

    if (ignoreObjects == false)
    {
        BaseObject obj = GetObject(cellPos);
        if (obj != null && obj != self)
            return false;
    }

    int x = cellPos.x - MinX;
    int y = MaxY - cellPos.y;
    ECellCollisionType type = _collision[x, y];
    if (type == ECellCollisionType.None)
        return true;

    if (ignoreSemiWall && type == ECellCollisionType.SemiWall)
        return true;

    return false;
}
```

---

## 대형몬스터를 찾게 A* 알고리즘 변수수정

* 대형몬스터의 중점을 위주로 찾는데 그게 맞나?
    * 문제는 보스몬스터 주변에 내 캐릭들이 들어가는경우 빈자리를 찾아가게 해야한다

```
 x x x
 x O v
 x v v

v - 여기에 내 캐릭들이 들어가게 해야함.
```

* 현재는 depth를 수정해서 보완 (이후 더 고려할 것)
    * A* 내부를 보면 알겠지만 Depth가 좀 돌아가더라도 목적지를 찾자임.

```csharp
/*
public const int HERO_DEFAULT_MOVE_DEPTH = 10;
public const int MONSTER_DEFAULT_MOVE_DEPTH = 3;
*/

FindPathAndMoveToCellPos(Target.transform.position, HERO_DEFAULT_MOVE_DEPTH);
```
