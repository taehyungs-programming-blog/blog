---
layout: default
title: "11. MapManger"
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

* 우선 이게 왜 필요하나 싶겠지만
* Object간 충돌 판정을 서버에서 하기위해서 필요하다
* 클라기준에선 Unity의 충돌을 사용하면 되지만 서버의 판별을 위해서 필요!

---

* 그래서 위에껄 누가 해주냐? `MapManger`!
* 우선, 갈수 있는 곳 없는곳을 그리기 위해서 Tile Palette를 이용해 그려놔야한다.
    * Window -> 2D -> Tile Palette

---

# How To Use ?

## LoadMap

```csharp
// 요렇게 호출 함.
Managers.Map.LoadMap("BaseMap");

// 구현
public void LoadMap(string mapName)
{
    // 기존 Map을 한번 비우고
    DestroyMap();

    // Resource Manager를 통해 Map을 생성
        // 내부는 Prefab으로 저장되어 있음.
    GameObject map = Managers.Resource.Instantiate(mapName);
    map.transform.position = Vector3.zero;
    map.name = $"@Map_{mapName}";

    Map = map;
    MapName = mapName;
    // Map에는 Grid가 붙어있다
    CellGrid = map.GetComponent<Grid>();

    // text로 collision 정보를 읽어 넣는다
    ParseCollisionData(map, mapName);

    SpawnObjectsByData(map, mapName);
}
```

```csharp
void ParseCollisionData(GameObject map, string mapName, string tilemap = "Tilemap_Collision")
{
    GameObject collision = Util.FindChild(map, tilemap, true);
    if (collision != null)
        collision.SetActive(false);

    // Collision Text 파일 로드 (Addressable로 관리 됨.)
    TextAsset txt = Managers.Resource.Load<TextAsset>($"{mapName}Collision");
    StringReader reader = new StringReader(txt.text);

    MinX = int.Parse(reader.ReadLine());
    MaxX = int.Parse(reader.ReadLine());
    MinY = int.Parse(reader.ReadLine());
    MaxY = int.Parse(reader.ReadLine());

    int xCount = MaxX - MinX + 1;
    int yCount = MaxY - MinY + 1;
    _collision = new ECellCollisionType[xCount, yCount];

    for (int y = 0; y < yCount; y++)
    {
        string line = reader.ReadLine();
        for (int x = 0; x < xCount; x++)
        {
            switch (line[x])
            {
                case Define.MAP_TOOL_WALL:
                    _collision[x, y] = ECellCollisionType.Wall;
                    break;
                case Define.MAP_TOOL_NONE:
                    _collision[x, y] = ECellCollisionType.None;
                    break;
                case Define.MAP_TOOL_SEMI_WALL:
                    _collision[x, y] = ECellCollisionType.SemiWall;
                    break;
            }
        }
    }
}
```

```csharp
void SpawnObjectsByData(GameObject map, string mapName, string tilemap = "Tilemap_Object")
{
    Tilemap tm = Util.FindChild<Tilemap>(map, tilemap, true);

    if (tm != null)
        // Tilemap은 Render를 멈춰야 안보인다.
        tm.gameObject.SetActive(false);

    return;
}
```

* 여기까지하면 로드는 완료!
    * 이제 어떻게 사용하는지 보자

---

## Cango

```csharp
public bool CanGo(Vector3Int cellPos, bool ignoreObjects = false, bool ignoreSemiWall = false)
{
    if (cellPos.x < MinX || cellPos.x > MaxX)
        return false;
    if (cellPos.y < MinY || cellPos.y > MaxY)
        return false;

    if (ignoreObjects == false)
    {
        BaseObject obj = GetObject(cellPos);
        if (obj != null)
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

* MoveTo도 비슷한 류라 정리는 하지 않는다

---

## GatherObject

```csharp
// 주변을 순회하며 Object를 모아준다

public List<T> GatherObjects<T>(Vector3 pos, float rangeX, float rangeY) where T : BaseObject
{
    List<T> objects = new List<T>();

    Vector3Int left = World2Cell(pos + new Vector3(-rangeX, 0));
    Vector3Int right = World2Cell(pos + new Vector3(+rangeX, 0));
    Vector3Int bottom = World2Cell(pos + new Vector3(0, -rangeY));
    Vector3Int top = World2Cell(pos + new Vector3(0, +rangeY));
    int minX = left.x;
    int maxX = right.x;
    int minY = bottom.y;
    int maxY = top.y;

    for (int x = minX; x <= maxX; x++)
    {
        for (int y = minY; y <= maxY; y++)
        {
            Vector3Int tilePos = new Vector3Int(x, y, 0);

            T obj = GetObject(tilePos) as T;
            if (obj == null)
                continue;

            objects.Add(obj);
        }
    }

    return objects;
}
```
