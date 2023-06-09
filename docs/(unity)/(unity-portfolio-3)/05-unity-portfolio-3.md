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
		// Skill Data로 부터 Skill 정보를 받아온다
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

	// OnTriggerEnter2D를 받기위해서 prefab에서 trigger를 켜놔야함 (주의)
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

---

* 쉽군 ... 
* 좀 눈여겨 봐야할 점은

```csharp
// PlayerController에서 Projectile을 소환하는데

IEnumerator CoStartProjectile()
{
	WaitForSeconds wait = new WaitForSeconds(0.5f);

	while (true)
	{
		// Manager의 Spawn을 사용한다(중앙에서 관리하기 위함)
		ProjectileController pc = Managers.Object.Spawn<ProjectileController>(_fireSocket.position, 1);
		pc.SetInfo(1, this, (_fireSocket.position - _indicator.position).normalized);

		yield return wait;
	}
}
```

```csharp
public T Spawn<T>(Vector3 position, int templateID = 0) where T : BaseController
{
	// ...
	else if (type == typeof(ProjectileController))
	{
		GameObject go = Managers.Resource.Instantiate("FireProjectile.prefab", pooling: true);
		go.transform.position = position;

		ProjectileController pc = go.GetOrAddComponent<ProjectileController>();
		Projectiles.Add(pc);
		pc.Init();

		return pc as T;
	}

	return null;
}
```

* 이런식으로 Manager에서 Object의 생성 및 삭제를 관리하는것에 유의!

