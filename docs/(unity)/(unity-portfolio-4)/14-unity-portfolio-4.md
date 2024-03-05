---
layout: default
title: "14. Skill System"
parent: "(포트폴리오 4 - 방치형 게임)"
grand_parent: "(Unity ✨)"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌎](https://github.com/Arthur880708/Unity.IncrementalGame.Example/tree/6)

---

# Data.Contents 변경

* Data.Contents의 내용이 변경됨, 관련해 복습 겸 정리.

```csharp
public class DataManager
{
	public Dictionary<int, Data.MonsterData> MonsterDic { get; private set; } = new Dictionary<int, Data.MonsterData>();
	// ...

    // Init 시점에 json을 읽는다
	public void Init()
	{
		MonsterDic = LoadJson<Data.MonsterDataLoader, int, Data.MonsterData>("MonsterData").MakeDict();
		HeroDic = LoadJson<Data.HeroDataLoader, int, Data.HeroData>("HeroData").MakeDict();
		SkillDic = LoadJson<Data.SkillDataLoader, int, Data.SkillData>("SkillData").MakeDict();
		ProjectileDic = LoadJson<Data.ProjectileDataLoader, int, Data.ProjectileData>("ProjectileData").MakeDict();
		EnvDic = LoadJson<Data.EnvDataLoader, int, Data.EnvData>("EnvData").MakeDict();
	}

	// ...
```

```csharp
public virtual void SetInfo(int templateID)
{
    DataTemplateID = templateID;

    // 그리고 SetInfo 시점에 데이터를 주입!
    if (CreatureType == ECreatureType.Hero)
        CreatureData = Managers.Data.HeroDic[templateID];
    else
        CreatureData = Managers.Data.MonsterDic[templateID];

    // ...
```

* 이걸 정리하는 이유는 SkillData를 읽어 쓰기때문이다.

---

# TODO : Skill System이 어떻게 돌아가나 확인

---

# TODO : 범위스킬 구현

---

# TODO: Damage Font 구현부

---

# TODO : 맞았을때 깜빡이는 효과