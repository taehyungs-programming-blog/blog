---
layout: default
title: "8.2. Search(정찰 및 길찾기)"
parent: "Link Page"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌍](https://github.com/EasyCoding-7/UnityPortfolio/tree/8.2.Search)

```csharp
public class MonsterController : CreatureController
{
    // ...

	IEnumerator CoSearch()
	{
		while (true)
		{
			yield return new WaitForSeconds(1);

			if (_target != null)
				continue;

			_target = Managers.Object.Find((go) =>
			{
				PlayerController pc = go.GetComponent<PlayerController>();
				if (pc == null)
					return false;

				Vector3Int dir = (pc.CellPos - CellPos);
				if (dir.magnitude > _searchRange)
					return false;

				return true;
			});
		}
	}

    // ...

    protected override void MoveToNextPos()
	{
		Vector3Int destPos = _destCellPos;
		if (_target != null)
		{
			destPos = _target.GetComponent<CreatureController>().CellPos;
		}
```


