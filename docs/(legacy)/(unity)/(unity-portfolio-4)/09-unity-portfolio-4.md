---
layout: default
title: "09. 캐릭터 AI"
parent: "(포트폴리오 4 - 방치형 게임)"
grand_parent: "(Unity ✨)"
great_grand_parent: "Legacy Archive"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌎](https://github.com/Arthur880708/Unity.IncrementalGame.Example/tree/3)

---

* Monster때와 유사하다

```csharp
public class Creature : BaseObject
    // ...

	public float UpdateAITick { get; protected set; } = 0.0f;

	protected IEnumerator CoUpdateAI()
	{
		while (true)
		{
			switch (CreatureState)
			{
				case ECreatureState.Idle:
					UpdateIdle();
					break;
				case ECreatureState.Move:
					UpdateMove();
					break;
				case ECreatureState.Skill:
					UpdateSkill();
					break;
				case ECreatureState.Dead:
					UpdateDead();
					break;
			}

			if (UpdateAITick > 0)
				yield return new WaitForSeconds(UpdateAITick);
			else
				yield return null;
		}
	}

    // 여기만 구현해주면 됨.
	protected virtual void UpdateIdle() { }
	protected virtual void UpdateMove() { }
	protected virtual void UpdateSkill() { }
	protected virtual void UpdateDead() { }

```

```csharp
public class Hero : Creature
{
    // ...

	public override bool Init()
	{
		// ...

        // Init 시점에 Coroutine Update 시작
		StartCoroutine(CoUpdateAI());

		return true;
	}
```

```csharp
public override ECreatureState CreatureState
{
	get { return _creatureState; }
	set
	{
		if (_creatureState != value)
		{
			base.CreatureState = value;

			switch (value)
			{
				// 미끄러지는 듯한 효과를 피하기 위해서 상태마다 Mass를 수정해 준다.
				case ECreatureState.Move:
					RigidBody.mass = CreatureData.Mass * 5.0f;
					break;
				case ECreatureState.Skill:
					RigidBody.mass = CreatureData.Mass * 500.0f;
					break;
				default:
					RigidBody.mass = CreatureData.Mass;
					break;
			}
		}
	}
}
```

* 내부로직은 생략

---

* 데미지 넣기
* 데미지는 피해를 받는 입장에서 넣으면 편하다

```csharp
// projectile에서 처리
private void OnTriggerEnter2D(Collider2D other)
{
    BaseObject target = other.GetComponent<BaseObject>();
    if (target.IsValid() == false)
        return;

    // TODO
    target.OnDamaged(Owner, Skill);
    Managers.Object.Despawn(this);
}
```
