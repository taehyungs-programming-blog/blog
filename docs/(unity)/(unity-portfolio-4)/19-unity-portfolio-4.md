---
layout: default
title: "19. Effect"
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

## TODO : 대표예시 Stun

* Effect\Buff, CC 를 확인
* `Airborne`, `CCBase` ...
* `AreaAirBone`


