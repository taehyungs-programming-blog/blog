---
layout: default
title: "8.1. Patrol"
parent: "Link Page"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌍](https://github.com/EasyCoding-7/UnityPortfolio/tree/8.1.Patrol)

---

```csharp
public class MonsterController : CreatureController
{
	Coroutine _coPatrol;
    
    // ...

	protected override void UpdateIdle()
	{
		base.UpdateIdle();

		if (_coPatrol == null)
		{
			_coPatrol = StartCoroutine("CoPatrol");
		}
	}

    // ...

    IEnumerator CoPatrol()
	{
		int waitSeconds = Random.Range(1, 4);
		yield return new WaitForSeconds(waitSeconds);

		for (int i = 0; i < 10; i++)
		{
			int xRange = Random.Range(-5, 6);
			int yRange = Random.Range(-5, 6);
			Vector3Int randPos = CellPos + new Vector3Int(xRange, yRange, 0);

			if (Managers.Map.CanGo(randPos) && Managers.Object.Find(randPos) == null)
			{
				_destCellPos = randPos;
				State = CreatureState.Moving;
				yield break;
			}
		}

		State = CreatureState.Idle;		
	}
```
