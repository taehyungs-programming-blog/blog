---
layout: default
title: "7.1. 스킬 이펙트"
parent: "Link Page"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌍](https://github.com/EasyCoding-7/UnityPortfolio/tree/7.1.Skill)

---

```csharp
public class PlayerController : CreatureController
{
	Coroutine _coSkill = null;

	protected override void Init()
	{
		base.Init();
	}

	protected override void UpdateController()
	{
		switch(State)
        {
			case CreatureState.Idle:
				GetDirInput();
				GetIdleInput();
				break;
			case CreatureState.Moving:
				GetDirInput();
				break;
		}

		
		base.UpdateController();
	}

	void GetIdleInput()
    {
		if (Input.GetKey(KeyCode.Space))
        {
			State = CreatureState.Skill;
			_coSkill = StartCoroutine("CoStartPunch");
		}
	}

	IEnumerator CoStartPunch()
    {
		// 피격
		GameObject go = Managers.Object.Find(GetFrontCellPos());
		if(go!= null)
        {
			Debug.Log(go.name);
        }
		// 대기 시간
		yield return new WaitForSeconds(0.5f);
		State = CreatureState.Idle;
		_coSkill = null;
    }
```

```csharp
public class CreatureController : MonoBehaviour
{
    // ...

	public Vector3Int GetFrontCellPos()
    {
		Vector3Int cellPos = CellPos;

		switch(_lastDir)
        {
			case MoveDir.Up:
				cellPos += Vector3Int.up;
				break;
			case MoveDir.Down:
				cellPos += Vector3Int.down;
				break;
			case MoveDir.Left:
				cellPos += Vector3Int.left;
				break;
			case MoveDir.Right:
				cellPos += Vector3Int.right;
				break;
		}

		return cellPos;
    }

	protected virtual void UpdateAnimation()
	{
		if (_state == CreatureState.Idle)
		{
			// ...
		else if (_state == CreatureState.Skill)
		{
			switch (_lastDir)
			{
				case MoveDir.Up:
					_animator.Play("ATTACK_BACK");
					_sprite.flipX = false;
					break;
				case MoveDir.Down:
					_animator.Play("ATTACK_FRONT");
					_sprite.flipX = false;
					break;
				case MoveDir.Left:
					_animator.Play("ATTACK_RIGHT");
					_sprite.flipX = true;
					break;
				case MoveDir.Right:
					_animator.Play("ATTACK_RIGHT");
					_sprite.flipX = false;
					break;
			}
		}
```