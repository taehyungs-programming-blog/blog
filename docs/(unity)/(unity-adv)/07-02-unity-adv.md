---
layout: default
title: "7.2. 화살 이펙트"
parent: "Link Page"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌍](https://github.com/EasyCoding-7/UnityPortfolio/tree/7.2.Arrow)

---

* 화살 컨트롤을 위해서 `ArrowController.cs` 생성

```csharp
public class ArrowController : CreatureController
{
	protected override void Init()
	{
		switch(_lastDir)
        {
			case MoveDir.Up:
				transform.rotation = Quaternion.Euler(0, 0, 0);
				break;
			case MoveDir.Down:
				transform.rotation = Quaternion.Euler(0, 0, -180);
				break;
			case MoveDir.Right:
				transform.rotation = Quaternion.Euler(0, 0, -90);
				break;
			case MoveDir.Left:
				transform.rotation = Quaternion.Euler(0, 0, 90);
				break;
		}
		base.Init();
	}

	protected override void UpdateAnimation()
	{

	}

	protected override void UpdateIdle()
	{
		if (_dir != MoveDir.None)
		{
			Vector3Int destPos = CellPos;

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

			State = CreatureState.Moving;
			if (Managers.Map.CanGo(destPos))
			{
				GameObject go = Managers.Object.Find(destPos);
				if (go == null)
				{
					CellPos = destPos;
				}
				else
                {
					Debug.Log(go.name);
					Managers.Resource.Destroy(gameObject);
				}
			}
			else
            {
				Managers.Resource.Destroy(gameObject);
            }
		}
	}
}
```

```csharp
void GetIdleInput()
{
    if (Input.GetKey(KeyCode.Space))
    {
        State = CreatureState.Skill;
        // _coSkill = StartCoroutine("CoStartPunch");
        _coSkill = StartCoroutine("CoStartShootArrow");
    }
}

IEnumerator CoStartShootArrow()
{
    GameObject go = Managers.Resource.Instantiate("Creature/Arrow");
    ArrowController ac = go.GetComponent<ArrowController>();
    ac.Dir = _lastDir;
    ac.CellPos = CellPos;
    _rangeSkill = true;
    // 대기 시간
    yield return new WaitForSeconds(0.3f);
    State = CreatureState.Idle;
    _coSkill = null;

}

// ...
```
