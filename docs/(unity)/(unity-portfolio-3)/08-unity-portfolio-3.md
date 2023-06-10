---
layout: default
title: "8. Boss AI"
parent: "(포트폴리오 3 - 뱀서 모작)"
grand_parent: "(Unity ✨)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* 크게 어렵진 않다

```csharp
public class BossController : MonsterController
{
	public override bool Init()
	{
		base.Init();

		_animator = GetComponent<Animator>();
		CreatureState = Define.CreatureState.Moving;
		Hp = 10000;

		return true;
	}

	public override void UpdateAnimation()
	{
		switch (CreatureState)
		{
			case Define.CreatureState.Idle:
				_animator.Play("Idle");
				break;
			case Define.CreatureState.Moving:
				_animator.Play("Moving");
				break;
			case Define.CreatureState.Skill:
				_animator.Play("Attack");
				break;
			case Define.CreatureState.Dead:
				_animator.Play("Death");
				break;
		}
	}

	// Boss Collider + Player Collider
	float _range = 2.0f;

	protected override void UpdateMoving()
	{
		PlayerController pc = Managers.Object.Player;
		if (pc.IsValid() == false)
			return;

		Vector3 dir = pc.transform.position - transform.position;

		if (dir.magnitude < _range)
		{
			CreatureState = Define.CreatureState.Skill;

			// _animator.runtimeAnimatorController.animationClips;
			float animLength = 0.41f;
			Wait(animLength);
		}
	}

	protected override void UpdateSkill() 
	{
		if (_coWait == null)
			CreatureState = Define.CreatureState.Moving;
	}

	protected override void UpdateDead() 
	{
		if (_coWait == null)
			Managers.Object.Despawn(this);
	}

	#region Wait Coroutine
	Coroutine _coWait;

	void Wait(float waitSeconds)
	{
		if (_coWait != null)
			StopCoroutine(_coWait);
		_coWait = StartCoroutine(CoStartWait(waitSeconds));
	}

	IEnumerator CoStartWait(float waitSeconds)
	{
		yield return new WaitForSeconds(waitSeconds);
		_coWait = null;
	}

	#endregion

	public override void OnDamaged(BaseController attacker, int damage)
	{
		base.OnDamaged(attacker, damage);
	}

	protected override void OnDead()
	{
		CreatureState = Define.CreatureState.Dead;
		Wait(2.0f);
	}
}
```
