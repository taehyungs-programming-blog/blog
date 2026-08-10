---
layout: default
title: "2.1. TileMap 기초"
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

* [Get Code 🌍](https://github.com/EasyCoding-7/UnityPortfolio/tree/2.1.Tilemap)

---

## TileMap 써보기

* **Scene 우클릭 -> 2D Object -> Tilemap**

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity-adv/unity-adv-2-1-1.png"/>
</p>

* Tilemap에 그리기 위해 Tile Palette가 필요
* **Window -> 2D -> Tile Palette**

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity-adv/unity-adv-2-1-2.png"/>
</p>

* TinyRPGTown의 tileset을 Drag and Drop 하자

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity-adv/unity-adv-2-1-3.png"/>
</p>

* 자신이 원하는 대로 그려보자.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity-adv/unity-adv-2-1-4.png"/>
</p>

---

## TileMap Layer 써보기

* 경우에 따라 Layer를 쌓아야 하는 경우가 발생한다.
* Tilemap을 복붙 후 Layer를 하나 올려주자

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity-adv/unity-adv-2-1-5.png"/>
</p>

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity-adv/unity-adv-2-1-6.png"/>
</p>

---

## TileMap Collider 써보기

* **Tilemap Collider 2D**를 Tilemap_Env에 Component로 넣는다

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity-adv/unity-adv-2-1-7.png"/>
</p>

* 자동으로 Collider가 추가된 모습을 볼 수 있다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity-adv/unity-adv-2-1-8.png"/>
</p>

* 그런데 이 방식은 Collider를 Sprite에 의존해야하기에 그리 좋지못한 방식이다.
* 개발자가 직접 Collider를 찍어줄수 있게 Tilemap을 하나 더 추가해 Collider를 넣어보자.
* Tilemap_Collision에 갈수없는 영역을 임의의 Sprite로 그려보자.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity-adv/unity-adv-2-1-9.png"/>
</p>

* 작은 돌로 갈수없는 영역을 지정함
* C#의 **TestCripts.cs**를 하나 만들고 Tilemap_Collision에  붙여보자.

```cs
// Tilemap변수를 하나 만들고 현재 사용중인 Tilemap을 붙여보자.
public class TestScripts : MonoBehaviour
{
    public Tilemap _tilemap;
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity-adv/unity-adv-2-1-10.png"/>
</p>

```cs
public class TestScripts : MonoBehaviour
{
    public Tilemap _tilemap;

    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        List<Vector3Int> blocked = new List<Vector3Int>();

        foreach(Vector3Int pos in _tilemap.cellBounds.allPositionsWithin)
        {
            // 갈수 없는 영역을 찾아본다.
            TileBase tile = _tilemap.GetTile(pos);
            if (tile != null)
                blocked.Add(pos);
        }
    }
}
```


