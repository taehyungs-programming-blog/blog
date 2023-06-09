---
layout: default
title: "6. Skill, Map 개선"
parent: "(포트폴리오 3 - 뱀서 모작)"
grand_parent: "(Unity ✨)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## 몬스터가 스폰되는 위치를 지정해 보자

```csharp
// Utils.cs

public static Vector2 GenerateMonsterSpawnPosition(Vector2 characterPosition, float minDistance = 10.0f, float maxDistance = 20.0f)
{
    float angle = Random.Range(0, 360) * Mathf.Deg2Rad;
    float distance = Random.Range(minDistance, maxDistance);

    float xDist = Mathf.Cos(angle) * distance;
    float yDist = Mathf.Sin(angle) * distance;

    Vector2 spawnPosition = characterPosition + new Vector2(xDist, yDist);

    return spawnPosition;
}
```

---

## 스킬을 하나 더 추가해 보자

```csharp
public class EgoSwordController : SkillController
{
    [SerializeField]
    ParticleSystem[] _swingParticles;

    protected enum SwingType
    {
        First,
        Second,
        Third,
        Fourth
    }

	public override bool Init()
	{
		base.Init();

		// Active 될때까지 콜라이더 물리적용 X
		for (int i = 0; i < _swingParticles.Length; i++)
			_swingParticles[i].GetComponent<Rigidbody2D>().simulated = false;

		for (int i = 0; i < _swingParticles.Length; i++)
			_swingParticles[i].gameObject.GetOrAddComponent<EgoSwordChild>().SetInfo(Managers.Object.Player, 100);

		return true;
	}

	public void ActivateSkill()
	{
		StartCoroutine(CoSwingSword());
	}

	float CoolTime = 2.0f;

	IEnumerator CoSwingSword()
	{
		while (true)
		{
			yield return new WaitForSeconds(CoolTime);

			SetParticles(SwingType.First);
			_swingParticles[(int)SwingType.First].Play();
			TurnOnPhysics(SwingType.First, true);
			yield return new WaitForSeconds(_swingParticles[(int)SwingType.First].main.duration);
			TurnOnPhysics(SwingType.First, false);

			SetParticles(SwingType.Second);
			_swingParticles[(int)SwingType.Second].Play();
			TurnOnPhysics(SwingType.Second, true);
			yield return new WaitForSeconds(_swingParticles[(int)SwingType.Second].main.duration);
			TurnOnPhysics(SwingType.Second, false);

			SetParticles(SwingType.Third);
			_swingParticles[(int)SwingType.Third].Play();
			TurnOnPhysics(SwingType.Third, true);
			yield return new WaitForSeconds(_swingParticles[(int)SwingType.Third].main.duration);
			TurnOnPhysics(SwingType.Third, false);

			SetParticles(SwingType.Fourth);
			_swingParticles[(int)SwingType.Fourth].Play();
			TurnOnPhysics(SwingType.Fourth, true);
			yield return new WaitForSeconds(_swingParticles[(int)SwingType.Fourth].main.duration);
			TurnOnPhysics(SwingType.Fourth, false);
		}
	}

	void SetParticles(SwingType swingType)
	{
		float z = transform.parent.transform.eulerAngles.z;
		float radian = (Mathf.PI / 180) * z * -1;

		var main = _swingParticles[(int)swingType].main;
		main.startRotation = radian;
	}

	private void TurnOnPhysics(SwingType swingType, bool simulated)
	{
		for (int i = 0; i < _swingParticles.Length; i++)
			_swingParticles[i].GetComponent<Rigidbody2D>().simulated = false;

		_swingParticles[(int)swingType].GetComponent<Rigidbody2D>().simulated = simulated;
	}
}
```

---

## 무한맵

* Map과 Camera에 Collision을 달아서 무한맵을 생성

```csharp
public class MapTileController : MonoBehaviour
{
	private void OnTriggerExit2D(Collider2D collision)
	{
		Camera camera = collision.gameObject.GetComponent<Camera>();
		if (camera == null)
			return;

		Vector3 dir = camera.transform.position - transform.position;

		float dirX = dir.x < 0 ? -1 : 1;
		float dirY = dir.y < 0 ? -1 : 1;

		if (Mathf.Abs(dir.x) > Mathf.Abs(dir.y))
			transform.Translate(Vector3.right * dirX * 200);
		else
			transform.Translate(Vector3.up * dirY * 200);
	}
}
```