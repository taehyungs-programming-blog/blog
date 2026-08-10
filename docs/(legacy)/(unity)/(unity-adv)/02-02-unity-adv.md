---
layout: default
title: "2.2. MapTool"
parent: "(Advanced)"
grand_parent: "(Unity ✨)"
great_grand_parent: "Legacy Archive"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌍](https://github.com/EasyCoding-7/UnityPortfolio/tree/2.2.MapTool)

---

* Map Editor를 만들어 보자.
* 우선 Map Editor의 손쉬운 접근을 위해서 Assets/Editor/MapEditor C# Script를 하나 만들고

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity-adv/unity-adv-2-2-1.png"/>
</p>

```cs
// UNITY_EDITOR는 라이브에 포함되지 않는 코드를 의미한다.
#if UNITY_EDITOR
using UnityEditor;
#endif

public class MapEditor : MonoBehaviour
{
#if UNITY_EDITOR

    // % (Ctrl), # (Shift), & (alt)
    [MenuItem("Tools/GenerateMap %#g")] // Ctrl + Shift + g
    private static void HelloWorld()
    {
        if(EditorUtility.DisplayDialog("Hello World", "Create?", "Create", "Cancel"))
        {
            // Scene에 GameObject를 하나 추가한다.
            new GameObject("Hello Wrold");
        }
    }

#endif
}
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity-adv/unity-adv-2-2-2.png"/>
</p>

```cs
public class MapEditor : MonoBehaviour
{
#if UNITY_EDITOR

    // % (Ctrl), # (Shift), & (alt)
    [MenuItem("Tools/GenerateMap %#g")] // Ctrl + Shift + g
    private static void GenerateMap()
    {
        if(EditorUtility.DisplayDialog("Hello World", "Create?", "Create", "Cancel"))
        {
            GameObject go = GameObject.Find("Map");
            if (go == null)
                return;

            Tilemap tm = Util.FindChild<Tilemap>(go, "Tilemap_Collision", true);

            List<Vector3Int> blocked = new List<Vector3Int>();

            foreach (Vector3Int pos in tm.cellBounds.allPositionsWithin)
            {
                TileBase tile = tm.GetTile(pos);
                if (tile != null)
                    blocked.Add(pos);
            }

            // 텍스트형태로 이동가능한 영역 불가능한 영역을 뽑자.
            using (var writer = File.CreateText("Assets/Resources/Map/output.txt"))
            {
                writer.WriteLine(tm.cellBounds.xMin);
                writer.WriteLine(tm.cellBounds.xMax);
                writer.WriteLine(tm.cellBounds.yMin);
                writer.WriteLine(tm.cellBounds.yMax);

                for(int y = tm.cellBounds.yMax; y >= tm.cellBounds.yMin; y--)
                {
                    for (int x = tm.cellBounds.xMin; x <= tm.cellBounds.xMax; x++)
                    {
                        TileBase tile = tm.GetTile(new Vector3Int(x, y, 0));
                        if(tile != null)
                        {
                            writer.Write(1);
                        }
                        else
                        {
                            writer.Write(0);
                        }
                    }
                    writer.WriteLine();
                }
            }
        }
    }

#endif
}

```

* Prefabs산하의 Map에 대해 모두 output.txt를 뽑게 변경해보자.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity-adv/unity-adv-2-2-3.png"/>
</p>


```cs
public class MapEditor : MonoBehaviour
{
#if UNITY_EDITOR

    // % (Ctrl), # (Shift), & (alt)
    [MenuItem("Tools/GenerateMap %#g")] // Ctrl + Shift + g
    private static void GenerateMap()
    {
        GameObject[] gameObjects = Resources.LoadAll<GameObject>("Prefabs/Map");

        foreach(GameObject go in gameObjects)
        {
            Tilemap tm = Util.FindChild<Tilemap>(go, "Tilemap_Collision", true);

            List<Vector3Int> blocked = new List<Vector3Int>();

            foreach (Vector3Int pos in tm.cellBounds.allPositionsWithin)
            {
                TileBase tile = tm.GetTile(pos);
                if (tile != null)
                    blocked.Add(pos);
            }

            using (var writer = File.CreateText($"Assets/Resources/Map/{go.name}.txt"))
            {
                writer.WriteLine(tm.cellBounds.xMin);
                writer.WriteLine(tm.cellBounds.xMax);
                writer.WriteLine(tm.cellBounds.yMin);
                writer.WriteLine(tm.cellBounds.yMax);

                for(int y = tm.cellBounds.yMax; y >= tm.cellBounds.yMin; y--)
                {
                    for (int x = tm.cellBounds.xMin; x <= tm.cellBounds.xMax; x++)
                    {
                        TileBase tile = tm.GetTile(new Vector3Int(x, y, 0));
                        if(tile != null)
                        {
                            writer.Write(1);
                        }
                        else
                        {
                            writer.Write(0);
                        }
                    }
                    writer.WriteLine();
                }
            }
        }
    }

#endif
}
```