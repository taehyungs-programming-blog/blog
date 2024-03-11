---
layout: default
title: "18. Stat System"
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

## CreatureStat

* 이제 Stat정보를 별도의 클래스로 관리한

```csharp
public class Creature : BaseObject
{
	// ....

	#region Stats
	public float Hp { get; set; }
	public CreatureStat MaxHp;
	public CreatureStat Atk;
	public CreatureStat CriRate;
	public CreatureStat CriDamage;
	public CreatureStat ReduceDamageRate;
	public CreatureStat LifeStealRate;
	public CreatureStat ThornsDamageRate; // 쏜즈
	public CreatureStat MoveSpeed;
	public CreatureStat AttackSpeedRate;
	#endregion
```

* 왜?
    * Effect를 어떻게 반영하냐에 대한 답이라 생각하면 된다.

```csharp
public class CreatureStat
{
	public float BaseValue { get; private set; }

	private bool _isDirty = true;

	[SerializeField]
	private float _value;
	public virtual float Value
	{
		get
		{
			if (_isDirty)
			{
                // Stat을 읽어올때 CalculateFinalValue()값을 리턴해주는
				_value = CalculateFinalValue();
				_isDirty = false;
			}
			return _value;
		}

		private set { _value = value; }
	}
```

```csharp
// 내부적으로 관리중인 StatModifiers의 결과를 리턴한다

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

    return (float)Math.Round(finalValue, 4);
}
```

---

## 실사용되는 예시를 보자

```csharp
public override void OnDamaged(BaseObject attacker, SkillBase skill)
{
    // ...

    // Creature가 OnDamaged를 입을 경우 GenerateEffects를 발동
    if (skill.SkillData.EffectIds != null)
        Effects.GenerateEffects(skill.SkillData.EffectIds.ToArray(), EEffectSpawnType.Skill, skill);
```

```csharp
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

```csharp
public class ReduceDmgBuff : BuffBase
{
	// ...
    
    // 버프류는 이런식으로 데이터를 넣음
    public override void ApplyEffect()
    {
        base.ApplyEffect();
        // Owner는 Creature이고 CreatureStat인 ReduceDamageRate를 AddModifier로 넘긴다
        // ReduceDamageRate에 BuffBase를 넣는것이다.
        AddModifier(Owner.ReduceDamageRate, this);
    }
```

---

## Tips) Stat 정보는 int를 쓸까 float을 쓸까

* 정수 데이터를 int와 float 중 어떤 타입으로 저장해야 하는지에 대한 질문에는 명확한 정답이 없지만, 여러 이유로 float를 사용하는 것이 더 유리할 수 있습니다. 
    * 우선, int는 정수만을 다루기 때문에, 연산 속도는 빠르지만(부동소수점이 아니기에) 연산 중에 소수점이 필요하거나 결과값으로 소수가 나올 경우에는 적절하지 않을 수 있습니다. 
    * 대부분의 연산, 예를 들어 퍼센트 계산 등에서는 결과값이 실수일 가능성이 높고, 이러한 경우에는 결과값을 float로 처리해야 하며, 이후에 결과값을 다시 정수로 변환하는 과정이 필요할 수 있습니다.
    * 또한, 향후 요구사항 변경이나 추가 기능 구현 시, 예를 들어 "초당 0.1의 체력을 추가한다"와 같은 기능이 추가될 경우, 이러한 기능은 실수 연산을 필요로 하기 때문에 처음부터 데이터를 float로 처리하는 것이 더 유연하고 확장성 있는 설계를 가능하게 합니다.
* 요약하자면, **초기에 정수 데이터를 float로 처리하는 것은 연산의 유연성, 확장성 및 향후 기능 추가를 고려할 때 더 유리**할 수 있습니다.
