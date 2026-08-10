---
layout: default
title: "14. Skill System"
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

* [Get Code 🌎](https://github.com/Arthur880708/Unity.IncrementalGame.Example/tree/6)

---

## Data.Contents 변경

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

## Skill System : 등록

```csharp
// 각 Creature는 SkillComponent를 갖는다
public class Creature : BaseObject
{
	// ...
	public SkillComponent Skills { get; protected set; }

	// ...

	public virtual void SetInfo(int templateID)
	{
		// ...

		// SetInfo를 하며 읽어온 CreatureData를 넣어준
		Skills = gameObject.GetOrAddComponent<SkillComponent>();
		Skills.SetInfo(this, CreatureData);
```

* CreatureData는 이렇게 구성되어 있다

```csharp
[Serializable]
public class CreatureData
{
	public int DataId;
	public string DescriptionTextID;
	public string PrefabLabel;
	public float ColliderOffsetX;
	public float ColliderOffsetY;
	public float ColliderRadius;
	public float MaxHp;
	public float UpMaxHpBonus;
	public float Atk;
	public float AtkRange;
	public float AtkBonus;
	public float MoveSpeed;
	public float CriRate;
	public float CriDamage;
	public string IconImage;
	public string SkeletonDataID;
	public int DefaultSkillId;
	public int EnvSkillId;
	public int SkillAId;
	public int SkillBId;
}
```

```csharp
// 스킬이 추가 될 시
public void AddSkill(int skillTemplateID, Define.ESkillSlot skillSlot)
{
	if (skillTemplateID == 0)
		return;

	if (Managers.Data.SkillDic.TryGetValue(skillTemplateID, out var data) == false)
	{
		Debug.LogWarning($"AddSkill Failed {skillTemplateID}");
		return;
	}

	// 해당 캐릭에 스킬이 붙이고
		// TIP) data.ClassName 이런식으로 string 타입캐스팅이 가능 (대박...)
	SkillBase skill = gameObject.AddComponent(Type.GetType(data.ClassName)) as SkillBase;
	if (skill == null)
		return;

	// 정보를 주입
	skill.SetInfo(_owner, skillTemplateID);

	SkillList.Add(skill);

	switch (skillSlot)
	{
		case Define.ESkillSlot.Default:
			DefaultSkill = skill;
			break;
		case Define.ESkillSlot.Env:
			EnvSkill = skill;
			break;
		case Define.ESkillSlot.A:
			ASkill = skill;
			ActiveSkills.Add(skill);
			break;
		case Define.ESkillSlot.B:
			BSkill = skill;
			ActiveSkills.Add(skill);
			break;
	}
}
```

```csharp
// skill 정보 주입
public virtual void SetInfo(Creature owner, int skillTemplateID)
{
	Owner = owner;
	SkillData = Managers.Data.SkillDic[skillTemplateID];

	// Register AnimEvent
	if (Owner.SkeletonAnim != null && Owner.SkeletonAnim.AnimationState != null)
	{
		Owner.SkeletonAnim.AnimationState.Event -= OnOwnerAnimEventHandler;
		Owner.SkeletonAnim.AnimationState.Event += OnOwnerAnimEventHandler;
	}
}
```

---

## Skill System : 애니메이션

```csharp
public virtual void DoSkill()
{
	if (Owner.Skills != null)
		Owner.Skills.ActiveSkills.Remove(this);

	float timeScale = 1.0f;

	if (Owner.Skills.DefaultSkill == this)
		Owner.PlayAnimation(0, SkillData.AnimName, false).TimeScale = timeScale;
	else
		Owner.PlayAnimation(0, SkillData.AnimName, false).TimeScale = 1;

	StartCoroutine(CoCountdownCooldown());
}
```

```csharp
public TrackEntry PlayAnimation(int trackIndex, string animName, bool loop)
{
	if (SkeletonAnim == null)
		return null;

	// SkeletonAnim.AnimationState.SetAnimation는 외부 라이브러리고
		// animName에 anim name을 넣으면 된다. 
		// anim name과 같은 spine정보는 디자인의 영역이다.
	TrackEntry entry = SkeletonAnim.AnimationState.SetAnimation(trackIndex, animName, loop);

	if (animName == AnimName.DEAD)
		entry.MixDuration = 0;
	else
		entry.MixDuration = 0.2f;

	return entry;
}
```
