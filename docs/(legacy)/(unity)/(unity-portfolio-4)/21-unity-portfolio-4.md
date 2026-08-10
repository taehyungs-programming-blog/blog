---
layout: default
title: "21. Area of Buff"
parent: "(포트폴리오 4 - 방치형 게임)"
grand_parent: "(Unity ✨)"
great_grand_parent: "Legacy Archive"
nav_order: 3
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌎](https://github.com/Arthur880708/Unity.IncrementalGame.Example/tree/8)

---

## 복습) Skill의 발동

```csharp
// SkillBase.cs

public virtual void GenerateAoE(Vector3 spawnPos)
{
    AoEBase aoe = null;
    int id = SkillData.AoEId;
    string className = Managers.Data.AoEDic[id].ClassName;

    Type componentType = Type.GetType(className);

    if (componentType == null)
    {
        Debug.LogError("AoE Type not found: " + className);
        return;
    }

    GameObject go = Managers.Object.SpawnGameObject(spawnPos, "AoE");
    go.name = Managers.Data.AoEDic[id].ClassName;
    aoe = go.AddComponent(componentType) as AoEBase;

    // aoe skill의 info를 주입
    aoe.SetInfo(SkillData.AoEId, Owner, this);
}
```

```csharp
public override void SetInfo(int dataId, BaseObject owner, SkillBase skill)
{
    base.SetInfo(dataId, owner, skill);

    SetSpineAnimation(_aoEData.SkeletonDataID, SortingLayers.SPELL_INDICATOR);
    // PlayAnimation 시에
    PlayAnimation(0, _aoEData.AnimName, false);

    StartCoroutine(CoReserveDestroy());
    StartCoroutine(CoDetectTargetsPeriodically());
}
```

```csharp
public TrackEntry PlayAnimation(int trackIndex, string animName, bool loop)
{
    if (SkeletonAnim == null)
        return null;

    // SkeletonAnim의 Anim이 실
    TrackEntry entry = SkeletonAnim.AnimationState.SetAnimation(trackIndex, animName, loop);

    if (animName == AnimName.DEAD)
        entry.MixDuration = 0;
    else
        entry.MixDuration = 0.2f;

    return entry;
}
```

```csharp
protected override void OnAttackEvent()
{
    float radius = Util.GetEffectRadius(SkillData.EffectSize);
    List<Creature> targets = Managers.Object.FindConeRangeTargets(Owner, _skillDir, radius, _angleRange);

    foreach (var target in targets)
    {
        if (target.IsValid())
        {
            // target들은 OnDamaged 가 호출되게 된다.
            target.OnDamaged(Owner, this);
        }
    }
}
```

```csharp
public override void OnDamaged(BaseObject attacker, SkillBase skill)
{
    // ...

    // AOE
    if (skill != null && skill.SkillData.AoEId != 0)
        skill.GenerateAoE(transform.position);
}
```

---

## 여기서 부터 Area of Effect

```csharp
// AoEBase.cs

protected void ApplyEffectsInRange(int angle)
{
    // 아군
    var allies = FindTargets(angle, true);
    ApplyEffectsToTargets(allies, _aoEData.AllyEffects.ToArray(), false);

    // 적군
    var enemies = FindTargets(angle, false);
    ApplyEffectsToTargets(enemies, _aoEData.EnemyEffects.ToArray(), true);
}

// ...

private void ApplyEffectsToTargets(List<Creature> targets, int[] effects, bool applyDamage)
{
    foreach (var target in targets)
    {
        Creature t = target as Creature;
        if (t.IsValid() == false)
            continue;

        t.Effects.GenerateEffects(effects, EEffectSpawnType.Skill, _skillBase);

        if (applyDamage)
            // Target에게 OnDamaged 호출.
            t.OnDamaged(Owner, _skillBase);
    }
}
```

---

## 장판 Buff는 어떻게 구현할까?

```csharp
// AoE에서 Target을 찾는다
private void DetectTargets()
{
    List<Creature> detectedCreatures = new List<Creature>();
    List<Creature> rangeTargets = Managers.Object.FindCircleRangeTargets(Owner, transform.position, _radius);

    foreach (var target in rangeTargets)
    {
        Creature t = target as Creature;

        detectedCreatures.Add(target);

        if (t.IsValid() && _targets.Contains(target) == false)
        {
            _targets.Add(t);

            // Effect data기반으로 주입
            List<EffectBase> effects = target.Effects.GenerateEffects(_aoEData.EnemyEffects.ToArray(), EEffectSpawnType.External, _skillBase);
            _activeEffects.AddRange(effects);
        }
    }

    // 범위체크
    foreach (var target in _targets.ToArray())
    {
        if (target.IsValid() && detectedCreatures.Contains(target) == false)
        {
            // 나감
            _targets.Remove(target);
            RemoveEffect(target);
        }
    }
}
```

```csharp
// 유의할 점은 Effect역시 EffectComponent로 관리된다는 점!

public class EffectComponent : MonoBehaviour
{
	public List<EffectBase> ActiveEffects = new List<EffectBase>();

    // ...

public List<EffectBase> GenerateEffects(IEnumerable<int> effectIds, EEffectSpawnType spawnType, SkillBase skill)
{
    List<EffectBase> generatedEffects = new List<EffectBase>();

    foreach (int id in effectIds)
    {
        string className = Managers.Data.EffectDic[id].ClassName;
        Type effectType = Type.GetType(className);

        if (effectType == null)
        {
            Debug.LogError($"Effect Type not found: {className}");
            return null;
        }

        GameObject go = Managers.Object.SpawnGameObject(_owner.CenterPosition, "EffectBase");			
        go.name = Managers.Data.EffectDic[id].ClassName;
        EffectBase effect = go.AddComponent(effectType) as EffectBase;
        effect.transform.parent = _owner.Effects.transform;
        effect.transform.localPosition = Vector2.zero;
        Managers.Object.Effects.Add(effect);

        ActiveEffects.Add(effect);
        generatedEffects.Add(effect);

        effect.SetInfo(id, _owner, spawnType, skill);
        effect.ApplyEffect();
    }

    return generatedEffects;
}
```