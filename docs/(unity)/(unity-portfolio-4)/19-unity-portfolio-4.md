---
layout: default
title: "19. Effect + 복습"
parent: "(포트폴리오 4 - 방치형 게임)"
grand_parent: "(Unity ✨)"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌎](https://github.com/Arthur880708/Unity.IncrementalGame.Example/tree/7)

---

## EffectBase

* JSON으로 된 EffectData를 읽어와 매니저에 기록한다

```csharp
// DataManager.cs
EffectDic = LoadJson<Data.EffectDataLoader, int, Data.EffectData>("EffectData").MakeDict();
```

```csharp
// EffectBase.cs
public virtual void SetInfo(int templateID, Creature owner, EEffectSpawnType spawnType, SkillBase skill)
{
    DataTemplateID = templateID;

    // Info주입시점에 Data를 넣어준다
    EffectData = Managers.Data.EffectDic[templateID];

    // ...
```

* 이제 EffectData가 어떻게 활용되나 AddModifier를 통해 보자

```csharp
protected void AddModifier(CreatureStat stat, object source, int order = 0)
{
    // CreatureStat에 Modifier를 붙여준다

    if (EffectData.Amount != 0)
    {
        StatModifier add = new StatModifier(EffectData.Amount, EStatModType.Add, order, source);
        stat.AddModifier(add);
    }

    if (EffectData.PercentAdd != 0)
    {
        StatModifier percentAdd = new StatModifier(EffectData.PercentAdd, EStatModType.PercentAdd, order, source);
        stat.AddModifier(percentAdd);
    }

    if (EffectData.PercentMult != 0)
    {
        StatModifier percentMult = new StatModifier(EffectData.PercentMult, EStatModType.PercentMult, order, source);
        stat.AddModifier(percentMult);
    }
}
```

```csharp
// CreatureStat.cs
// Modifier는 Value계산시 계산되어 리턴
private float CalculateFinalValue()
{
    float finalValue = BaseValue;
    float sumPercentAdd = 0;

    StatModifiers.Sort(CompareOrder);

    for (int i = 0; i < StatModifiers.Count; i++)
    {
        StatModifier modifier = StatModifiers[i];

        switch (modifier.Type)
        {
            case EStatModType.Add:
                finalValue += modifier.Value;
                break;
            case EStatModType.PercentAdd:
                sumPercentAdd += modifier.Value;
                if (i == StatModifiers.Count - 1 || StatModifiers[i + 1].Type != EStatModType.PercentAdd)
                {
                    finalValue *= 1 + sumPercentAdd;
                    sumPercentAdd = 0;
                }
                break;
            case EStatModType.PercentMult:
                finalValue *= 1 + modifier.Value;
                break;
        }
    }
```

---

## CC Effect 학습 전 약간?의 복습

* Skill이 어떻게 발되는가

```csharp
// Creature.cs

protected IEnumerator CoUpdateAI()
{
    while (true)
    {
        // CreatureState의 state를 보고 발동된다.
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
            case ECreatureState.OnDamaged:
                UpdateOnDamaged();
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
```

```csharp
// 공격범위 내로 들어올 경우 State를 변경하는 식으로 동작

protected void ChaseOrAttackTarget(float chaseRange, float attackRange)
{
    float distToTargetSqr = DistToTargetSqr;
    float attackDistanceSqr = attackRange * attackRange;

    if (distToTargetSqr <= attackDistanceSqr)
    {
        // 공격 범위 이내로 들어왔다면 공격.
        CreatureState = ECreatureState.Skill;

        // ...
```

* 이렇게 하면 스킬은 발동됨.
* 그렇다면 스킬은 어떤식으로 구성이 되어 있나

```
        InitBase
           |
       SkillBase   <----------  SkillComponent  <---------- Creature
           |
        -------
        |      |
  AreaSkill   NoramlAttack
```

* Creature에서 SkillComponent로 Skill을 관리한다
    * 이런식으로 하는 이유는 현재 발동중인 Skill정보를 알아야하기 때문!

```csharp
// SkillComponent.cs 
    // 아래와 같은식

public SkillBase CurrentSkill
{
    get
    {
        if (ActiveSkills.Count == 0)
            return DefaultSkill;

        int randomIndex = Random.Range(0, ActiveSkills.Count);
        return ActiveSkills[randomIndex];
    }
}
```

---

## CCBase Class가 어떻게 동작하나 확인하면 가장 정확하다

```
// 현재구현이 아래와 같이 되어있다

        EffectBase
            |
         CCBase
            |
      --------------
      |      |     |
    Freeze  Stun   Airborne
```

* Skill의 발동은

```csharp
// Creature.cs

public override void OnDamaged(BaseObject attacker, SkillBase skill)
{
    // ...

    // 들어온 스킬을 발동
    if (skill.SkillData.EffectIds != null)
        Effects.GenerateEffects(skill.SkillData.EffectIds.ToArray(), EEffectSpawnType.Skill, skill);

    // ...
```

```csharp
// CCBase.cs

public override void ApplyEffect()
{
    base.ApplyEffect();

    lastState = Owner.CreatureState;
    if (lastState == ECreatureState.OnDamaged)
        return;

    Owner.CreatureState = ECreatureState.OnDamaged;
}
```

```csharp
// Airborn.cs 에서 ApplyEffect 발동

public override void ApplyEffect()
{
    base.ApplyEffect();

    StopCoroutine((DoAirborn(lastState)));
    StartCoroutine(DoAirborn(lastState));
}

IEnumerator DoAirborn(ECreatureState lastState)
{
    Vector3 originalPosition = Owner.SkeletonAnim.transform.localPosition;
    Vector3 upPosition = originalPosition + Vector3.up * _airborneDistance;

    float halfTickTime = EffectData.TickTime * 0.5f;

    // up
    for (float t = 0; t < halfTickTime; t += Time.deltaTime)
    {
        float normalizedTime = t / halfTickTime;
        Owner.SkeletonAnim.transform.localPosition = Vector3.Lerp(originalPosition, upPosition, normalizedTime);
        yield return null;
    }

    // down
    for (float t = 0; t < halfTickTime; t += Time.deltaTime)
    {
        float normalizedTime = t / halfTickTime;
        Owner.SkeletonAnim.transform.localPosition = Vector3.Lerp(upPosition, originalPosition, normalizedTime);
        yield return null;
    }

    Owner.SkeletonAnim.transform.localPosition = originalPosition;

    // 복귀
    if (Owner.CreatureState == ECreatureState.OnDamaged)
        Owner.CreatureState = ECreatureState.Idle;

    ClearEffect(EEffectClearType.EndOfAirborne);
}
```