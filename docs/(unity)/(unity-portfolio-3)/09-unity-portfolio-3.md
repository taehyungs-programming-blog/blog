---
layout: default
title: "9. Skill 2"
parent: "(포트폴리오 3 - 뱀서 모작)"
grand_parent: "(Unity ✨)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* 목표 : 스킬의 처리를 하나의 Object에서 처리하고 싶다

```csharp
// SkillBook에 모든 Skilll을 담을 예정
public class SkillBook : MonoBehaviour
{
	public List<SkillBase> Skills { get; } = new List<SkillBase>();
	public List<SkillBase> RepeatedSkills { get; } = new List<SkillBase>();
	public List<SequenceSkill> SequenceSkills { get; } = new List<SequenceSkill>();
    
    public T AddSkill<T>(Vector3 position, Transform parent = null) where T : SkillBase
    {
        System.Type type = typeof(T);

		if (type == typeof(EgoSword))
        {
			var egoSword = Managers.Object.Spawn<EgoSword>(position, Define.EGO_SWORD_ID);
			egoSword.transform.SetParent(parent);
			egoSword.ActivateSkill();

			Skills.Add(egoSword);
			RepeatedSkills.Add(egoSword);

			return egoSword as T;
		}
        else if (type == typeof(FireballSkill))
        {
			var fireball = Managers.Object.Spawn<FireballSkill>(position, Define.EGO_SWORD_ID);
            fireball.transform.SetParent(parent);
			fireball.ActivateSkill();

			Skills.Add(fireball);
			RepeatedSkills.Add(fireball);

			return fireball as T;
		}
		else if (type.IsSubclassOf(typeof(SequenceSkill)))
		{
			var skill = gameObject.GetOrAddComponent<T>();
			Skills.Add(skill); 
			SequenceSkills.Add(skill as SequenceSkill);

			return null;
		}

		return null;
    }

	int _sequenceIndex = 0;

	public void StartNextSequenceSkill()
	{
		if (_stopped)
			return;
		if (SequenceSkills.Count == 0)
			return;
		
        // DoSkill이 발동되면 OnFinishedSequenceSkill가 호출되며
        // 다음 Skill로 넘어간다
		SequenceSkills[_sequenceIndex].DoSkill(OnFinishedSequenceSkill);
	}

	void OnFinishedSequenceSkill()
	{
		_sequenceIndex = (_sequenceIndex + 1) % SequenceSkills.Count;
		StartNextSequenceSkill();
	}

	bool _stopped = false;

	public void StopSkills()
	{
		_stopped = true;

		foreach (var skill in RepeatedSkills)
		{
			skill.StopAllCoroutines();
		}
	}
}
```

```csharp
// 예를들어 Boss는 Skill을 아래와 같이 등록한다

public class BossController : MonsterController
{
	public override bool Init()
	{
		base.Init();

		_animator = GetComponent<Animator>();
		CreatureState = Define.CreatureState.Moving;
		Hp = 10000;

		CreatureState = Define.CreatureState.Skill;
		Skills.AddSkill<Move>(transform.position);
		Skills.AddSkill<Dash>(transform.position);
		Skills.AddSkill<Dash>(transform.position);
		Skills.AddSkill<Dash>(transform.position);
		Skills.StartNextSequenceSkill();

		return true;
	}

    // ..
```

---

* 각 Skill은 어떻게 구성이 되어있을까? 보아하니 SkillBase기반인거 같은데

```csharp
public class SkillBase : BaseController
{
	public CreatureController Owner { get; set; }
	public Define.SkillType SkillType { get; set; } = Define.SkillType.None;
    public Data.SkillData SkillData { get; protected set; }

	public int SkillLevel { get; set; } = 0;
	public bool IsLearnedSkill { get { return SkillLevel > 0; } }

	public int Damage { get; set; } = 100;

	public SkillBase(Define.SkillType skillType)
	{
		SkillType = skillType;
	}

	public virtual void ActivateSkill()
	{

	}

	protected virtual void GenerateProjectile(int templateID, CreatureController owner, Vector3 startPos, Vector3 dir, Vector3 targetPos)
	{
		ProjectileController pc = Managers.Object.Spawn<ProjectileController>(startPos, templateID);
		pc.SetInfo(templateID, owner, dir);
	}

	#region Destroy
	Coroutine _coDestroy;

	public void StartDestroy(float delaySeconds)
	{
		StopDestroy();
		_coDestroy = StartCoroutine(CoDestroy(delaySeconds));
	}

	public void StopDestroy()
	{
		if (_coDestroy != null)
		{
			StopCoroutine(_coDestroy);
			_coDestroy = null;
		}
	}

	IEnumerator CoDestroy(float delaySeconds)
	{
		yield return new WaitForSeconds(delaySeconds);

		if (this.IsValid())
		{
			Managers.Object.Despawn(this);
		}
	}

	#endregion
}
```

```csharp
public class Move : SequenceSkill
{
	Rigidbody2D _rb;
	Coroutine _coroutine;

	private void Awake()
	{

	}

	public override void DoSkill(Action callback = null)
	{
		if (_coroutine != null)
			StopCoroutine(_coroutine);

		_coroutine = StartCoroutine(CoMove(callback));
	}

	float Speed { get; } = 2.0f;
	string AnimationName { get; } = "Moving";

	IEnumerator CoMove(Action callback = null)
	{
		_rb = GetComponent<Rigidbody2D>();
		GetComponent<Animator>().Play(AnimationName);
		float elapsed = 0;

		while (true)
		{
			elapsed += Time.deltaTime;
			if (elapsed > 5.0f)
				break;

			Vector3 dir = ((Vector2)Managers.Game.Player.transform.position - _rb.position).normalized;
			Vector2 targetPosition = Managers.Game.Player.transform.position + dir * UnityEngine.Random.Range(1, 4);

			if (Vector3.Distance(_rb.position, targetPosition) <= 0.2f)
				continue;

			Vector2 dirVec = targetPosition - _rb.position;
			Vector2 nextVec = dirVec.normalized * Speed * Time.fixedDeltaTime;
			_rb.MovePosition(_rb.position + nextVec);

			yield return null;
		}

		callback?.Invoke();
	}
}
```

---

* 다시 간단 정리

```csharp
// Creature는 무조건 Skill을 갖는다
public class CreatureController : BaseController
{
    protected float _speed = 1.0f;

    public int Hp { get; set; } = 100;
    public int MaxHp { get; set; } = 100;

	public SkillBook Skills { get; protected set; }

	public override bool Init()
	{
		base.Init();

        // 무조건 Skill포함
		Skills = gameObject.GetOrAddComponent<SkillBook>();

		return true;
	}

    // ...
```

```csharp
public class PlayerController : CreatureController
{
    // ...

    // Skill은 이렇게 적용
	public override bool Init()
	{
        if (base.Init() == false)
            return false;

        _speed = 5.0f;
        Managers.Game.OnMoveDirChanged += HandleOnMoveDirChanged;

		Skills.AddSkill<FireballSkill>(transform.position);
		Skills.AddSkill<EgoSword>(_indicator.position);

		return true;
	}
```