---
layout: default
title: "4. Grid를 이용한 떨어진 Item 최적화"
parent: "(포트폴리오 3 - 뱀서 모작)"
grand_parent: "(Unity ✨)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

```csharp
void CollectEnv()
{
	// 이런식으로 거리를 구하는게 sqrt
	float sqrCollectDist = EnvCollectDist * EnvCollectDist;

	List<GemController> gems = Managers.Object.Gems.ToList();
	
	// 여기서 Gem을 한번 더 찾는 이유는 뭘까?
	var findGems = GameObject.Find("@Grid").GetComponent<GridController>().GatherObjects(transform.position, EnvCollectDist + 0.5f);

	// ...
```

* 모든 Gem의 Collect를 판별하는것은 효율적이지 못하다
* Grid를 이용해 내 Grid내의 Gem만 Collect 판정을 하려한다

* 그럼 우선 Grid를 알아야겠지?
* Unity에서 지원해 주는 Grid Component이다.
	* 말 그대로 Component이기에 Object에 추가하는 형태로 사용이 가능하다

```csharp
class Cell
{
    public HashSet<GameObject> Objects { get; } = new HashSet<GameObject>();
}

public class GridController : BaseController
{
    UnityEngine.Grid _grid;

    Dictionary<Vector3Int, Cell> _cells = new Dictionary<Vector3Int, Cell>();

	public override bool Init()
	{
		base.Init();

		_grid = gameObject.GetOrAddComponent<UnityEngine.Grid>();

		return true;
	}

	public void Add(GameObject go)
	{
		// 현재 gameobject의 위치를 cell로 구한다
		Vector3Int cellPos = _grid.WorldToCell(go.transform.position);

		// cell이 있는지 확인 후 없다면 생성하고
		Cell cell = GetCell(cellPos);
		if (cell == null)
			return;

		// cell을 Dictionary에 저장
		cell.Objects.Add(go);
	}

	public void Remove(GameObject go) 
	{
		Vector3Int cellPos = _grid.WorldToCell(go.transform.position);

		Cell cell = GetCell(cellPos);
		if (cell == null)
			return;

		cell.Objects.Remove(go);
	}

	Cell GetCell(Vector3Int cellPos)
	{
		Cell cell = null;

		if (_cells.TryGetValue(cellPos, out cell) == false)
		{
			cell = new Cell();
			_cells.Add(cellPos, cell);
		}

		return cell;
	}

	public List<GameObject> GatherObjects(Vector3 pos, float range)
	{
		List<GameObject> objects = new List<GameObject>();

		Vector3Int left = _grid.WorldToCell(pos + new Vector3(-range, 0));
		Vector3Int right = _grid.WorldToCell(pos + new Vector3(+range, 0));
		Vector3Int bottom = _grid.WorldToCell(pos + new Vector3(0, -range));
		Vector3Int top = _grid.WorldToCell(pos + new Vector3(0, +range));

		int minX = left.x;
		int maxX = right.x;
		int minY = bottom.y;
		int maxY = top.y;

		// 자신 주변의 cell을 모두 모은다
		for (int x = minX; x <= maxX; x++)
		{
			for (int y = minY; y <= maxY; y++)
			{
				if (_cells.ContainsKey(new Vector3Int(x, y, 0)) == false)
					continue;

				objects.AddRange(_cells[new Vector3Int(x, y, 0)].Objects);
			}
		}

		return objects;
	}
}
```