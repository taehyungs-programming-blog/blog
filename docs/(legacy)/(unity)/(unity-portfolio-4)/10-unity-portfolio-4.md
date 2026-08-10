---
layout: default
title: "10. 중간정리"
parent: "(포트폴리오 4 - 방치형 게임)"
grand_parent: "(Unity ✨)"
great_grand_parent: "Legacy Archive"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌎](https://github.com/Arthur880708/Unity.IncrementalGame.Example/tree/3)

---

## Init

```csharp
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using static Define;

public class UI_TitleScene : UI_Scene
{
    // ...

    public override bool Init()
    {
        if (base.Init() == false)
            return false;

        BindObjects(typeof(GameObjects));
        BindTexts(typeof(Texts));

		GetObject((int)GameObjects.StartImage).BindEvent((evt) =>
		{
			Debug.Log("ChangeScene");
			Managers.Scene.LoadScene(EScene.GameScene);
		});

		GetObject((int)GameObjects.StartImage).gameObject.SetActive(false);
		GetText((int)Texts.DisplayText).text = $"";

		StartLoadAssets();

		return true;
    }

    // 시작과 동시에 Addressable을 통해 리소스를 로드한다.
	void StartLoadAssets()
	{
		Managers.Resource.LoadAllAsync<Object>("PreLoad", (key, count, totalCount) =>
		{
			Debug.Log($"{key} {count}/{totalCount}");

			if (count == totalCount)
			{
				Managers.Data.Init();
/*
DataManager의 JSON 데이터에 담아둔다
public class DataManager
// ...
public void Init()
{
    MonsterDic = LoadJson<Data.MonsterDataLoader, int, Data.MonsterData>("MonsterData").MakeDict();
    HeroDic = LoadJson<Data.HeroDataLoader, int, Data.HeroData>("HeroData").MakeDict();
    SkillDic = LoadJson<Data.SkillDataLoader, int, Data.SkillData>("SkillData").MakeDict();
    ProjectileDic = LoadJson<Data.ProjectileDataLoader, int, Data.ProjectileData>("ProjectileData").MakeDict();
    EnvDic = LoadJson<Data.EnvDataLoader, int, Data.EnvData>("EnvData").MakeDict();
}
*/

// ...
```

---

## Spawn

* Object의 관리는 `ObjectManager`에서 담당한다.

```csharp
public class GameScene : BaseScene
{
	public override bool Init()
	{
		if (base.Init() == false)
			return false;

		SceneType = EScene.GameScene;

		GameObject map = Managers.Resource.Instantiate("BaseMap");
		map.transform.position = Vector3.zero;
		map.name = "@BaseMap";

		HeroCamp camp = Managers.Object.Spawn<HeroCamp>(new Vector3Int(-10, -5, 0), 0);

		for (int i = 0; i < 5; i++)
		{
			//int heroTemplateID = HERO_WIZARD_ID + Random.Range(0, 5);
			//int heroTemplateID = HERO_KNIGHT_ID;
			int heroTemplateID = HERO_WIZARD_ID;
			Hero hero = Managers.Object.Spawn<Hero>(new Vector3Int(-10 + Random.Range(-5, 5), -5 + Random.Range(-5, 5), 0), heroTemplateID);
		}
```

```csharp
public T Spawn<T>(Vector3 position, int templateID) where T : BaseObject
{
    string prefabName = typeof(T).Name;

    GameObject go = Managers.Resource.Instantiate(prefabName);
    go.name = prefabName;
    go.transform.position = position;

    BaseObject obj = go.GetComponent<BaseObject>();

    // ...
```

---

## Skill System

* SkillComponent에 보유 스킬을 등록

```csharp
// public class InitBase : MonoBehaviour
    // MonoBehaviour에 붙여 그냥 캐릭에 붙인다
public class SkillComponent : InitBase
{
	public List<SkillBase> SkillList { get; } = new List<SkillBase>();

	Creature _owner;

    // ...
```

```csharp
public class Hero : Creature
{
    // ...
	public override void SetInfo(int templateID)
	{
		base.SetInfo(templateID);

		// State
		CreatureState = ECreatureState.Idle;

		// Skill
		Skills = gameObject.GetOrAddComponent<SkillComponent>();
		Skills.SetInfo(this, CreatureData.SkillIdList);
	}
```

* 스킬을 발동

```csharp
if (HeroMoveState == EHeroMoveState.TargetMonster)
{
    // ...

    SkillBase skill = Skills.GetReadySkill();
    // ChaseOrAttackTarget는 크리처내부에 있음.
    ChaseOrAttackTarget(HERO_SEARCH_DISTANCE, skill);   // 여기로 스킬을 넘긴다
    return;
}
```

* 예시를 들어보자

```csharp
public class NormalAttack : SkillBase
{
	public override bool Init()
	{
		// ...
	}

	public override void SetInfo(Creature owner, int skillTemplateID)
	{
		// ...
	}

	public override void DoSkill()
	{
		base.DoSkill();

		Owner.CreatureState = Define.ECreatureState.Skill;
		Owner.PlayAnimation(0, SkillData.AnimName, false);

		Owner.LookAtTarget(Owner.Target);
	}

	protected override void OnAnimEventHandler(TrackEntry trackEntry, Spine.Event e)
	{
		if (e.ToString().Contains(SkillData.AnimName))
			OnAttackEvent();
	}

	void PickupTargetAndProcessHit()
	{
	}

	protected virtual void OnAttackEvent()
	{
		if (Owner.Target.IsValid() == false)
			return;

		if (SkillData.ProjectileId == 0)
		{
			Owner.Target.OnDamaged(Owner, this);
		}
		else
		{
			GenerateProjectile(Owner, Owner.CenterPosition);
		}
	}

	protected override void OnAnimCompleteHandler(TrackEntry trackEntry)
	{
		if (Owner.Target.IsValid() == false)
			return;

		if (Owner.CreatureState == Define.ECreatureState.Skill)
			Owner.CreatureState = Define.ECreatureState.Move;
	}	
}
```

---

## Projectile

```csharp
// OnAttackEvent 호출시
protected virtual void OnAttackEvent()
{
    if (Owner.Target.IsValid() == false)
        return;

    if (SkillData.ProjectileId == 0)
    {
        // Melee
        Owner.Target.OnDamaged(Owner, this);
    }
    else
    {
        // Ranged
        GenerateProjectile(Owner, Owner.CenterPosition);
    }
}
```

```csharp
protected virtual void GenerateProjectile(Creature owner, Vector3 spawnPos)
{
    Projectile projectile = Managers.Object.Spawn<Projectile>(spawnPos, SkillData.ProjectileId);

    // ...

    projectile.SetSpawnInfo(Owner, this, excludeMask);
}
```