---
layout: default
title: "4. MapManager"
parent: "(Advanced)"
grand_parent: "(Unity ✨)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌍](https://github.com/EasyCoding-7/UnityPortfolio/tree/4.MapManager)

* 현재 해결이 필요한 부분은 충돌처리, 화면을 벗어날시 처리이다.
* 우선 충돌처리를 먼저해보자면.

```csharp
public class MapManager
{
	public Grid CurrentGrid { get; private set; }

	public int MinX { get; set; }
	public int MaxX { get; set; }
	public int MinY { get; set; }
	public int MaxY { get; set; }

	bool[,] _collision;

	public bool CanGo(Vector3Int cellPos)
	{
		if (cellPos.x < MinX || cellPos.x > MaxX)
			return false;
		if (cellPos.y < MinY || cellPos.y > MaxY)
			return false;

		int x = cellPos.x - MinX;
		int y = MaxY - cellPos.y;
		return !_collision[y, x];
	}

	public void LoadMap(int mapId)
	{
		DestroyMap();

		string mapName = "Map_" + mapId.ToString("000");
		GameObject go = Managers.Resource.Instantiate($"Map/{mapName}");
		go.name = "Map";

		GameObject collision = Util.FindChild(go, "Tilemap_Collision", true);
		if (collision != null)
			collision.SetActive(false);

		CurrentGrid = go.GetComponent<Grid>();

		// Collision 관련 파일
		TextAsset txt = Managers.Resource.Load<TextAsset>($"Map/{mapName}");
		StringReader reader = new StringReader(txt.text);

		MinX = int.Parse(reader.ReadLine());
		MaxX = int.Parse(reader.ReadLine());
		MinY = int.Parse(reader.ReadLine());
		MaxY = int.Parse(reader.ReadLine());

		int xCount = MaxX - MinX + 1;
		int yCount = MaxY - MinY + 1;
		_collision = new bool[yCount, xCount];

		for (int y = 0; y < yCount; y++)
		{
			string line = reader.ReadLine();
			for (int x = 0; x < xCount; x++)
			{
				_collision[y, x] = (line[x] == '1' ? true : false);
			}
		}
	}

	public void DestroyMap()
	{
		GameObject map = GameObject.Find("Map");
		if (map != null)
		{
			GameObject.Destroy(map);
			CurrentGrid = null;
		}
	}
}
```

```csharp
public class PlayerController : MonoBehaviour
{
    // ...

	// 이동 가능한 상태일 때, 실제 좌표를 이동한다
	void UpdateIsMoving()
	{
		if (_isMoving == false && _dir != MoveDir.None)
		{
			Vector3Int destPos = _cellPos;

			switch (_dir)
			{
				case MoveDir.Up:
					destPos += Vector3Int.up;
					break;
				case MoveDir.Down:
					destPos += Vector3Int.down;
					break;
				case MoveDir.Left:
					destPos += Vector3Int.left;
					break;
				case MoveDir.Right:
					destPos += Vector3Int.right;
					break;
			}

			if (Managers.Map.CanGo(destPos))
			{
				_cellPos = destPos;
				_isMoving = true;
			}
		}
	}
}
```

* 이제 화면을 벗어나면 어떻게 처리할지 

```csharp
public class PlayerController : MonoBehaviour
{
    // ...

	void LateUpdate()
	{
		Camera.main.transform.position = new Vector3(transform.position.x, transform.position.y, -10);
	}
```