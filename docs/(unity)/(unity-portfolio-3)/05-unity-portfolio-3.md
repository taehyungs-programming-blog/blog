---
layout: default
title: "5. Skill"
parent: "(포트폴리오 3 - 뱀서 모작)"
grand_parent: "(Unity ✨)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* Skill 데이터 정리법

```csharp
// DataManager에서 SkillData.xml로드

public class DataManager
{
	public Dictionary<int, Data.PlayerData> PlayerDic { get; private set; } = new Dictionary<int, Data.PlayerData>();
	public Dictionary<int, Data.SkillData> SkillDic { get; private set; } = new Dictionary<int, Data.SkillData>();

	public void Init()
	{
		//PlayerDic = LoadJson<Data.PlayerDataLoader, int, Data.PlayerData>("PlayerData.json").MakeDict();
		PlayerDic = LoadXml<Data.PlayerDataLoader, int, Data.PlayerData>("PlayerData.xml").MakeDict();
		SkillDic = LoadXml<Data.SkillDataLoader, int, Data.SkillData>("SkillData.xml").MakeDict();
	}
```

```csharp
public class SkillController : BaseController
{
    public Define.SkillType SkillType { get; set; }
    public Data.SkillData SkillData { get; protected set; }

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
public class ProjectileController : SkillController
{
    CreatureController _owner;
    Vector3 _moveDir;
    float _speed = 10.0f;
    float _lifeTime = 10.0f;

	public override bool Init()
	{
		base.Init();

		StartDestroy(_lifeTime);

		return true;
	}

	public void SetInfo(int templateID, CreatureController owner, Vector3 moveDir)
	{
		if (Managers.Data.SkillDic.TryGetValue(templateID, out Data.SkillData data) == false) 
		{
			Debug.LogError("ProjecteController SetInfo Failed");
			return;
		}

		_owner = owner;
		_moveDir = moveDir;
		SkillData = data;
		// TODO : Data Parsing
	}

	public override void UpdateController()
	{
		base.UpdateController();

		transform.position += _moveDir * _speed * Time.deltaTime;
	}

	void OnTriggerEnter2D(Collider2D collision)
	{
		MonsterController mc = collision.gameObject.GetComponent<MonsterController>();
		if (mc.IsValid() == false)
			return;
		if (this.IsValid() == false)
			return;

		mc.OnDamaged(_owner, SkillData.damage);

		StopDestroy();

		Managers.Object.Despawn(this);
	}
}
```