---
layout: default
title: "08. 캐릭터 군집제어"
parent: "(포트폴리오 4 - 방치형 게임)"
grand_parent: "(Unity ✨)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

```csharp
public class GameScene : BaseScene
{
	public override bool Init()
	{
		// ...

        // HeroCamp를 만들고 주변으로 군집하게 하고자 한다.
		HeroCamp camp = Managers.Object.Spawn<HeroCamp>(new Vector3Int(-10, -5, 0), 0);

		for (int i = 0; i < 5; i++)
		{
			Hero hero = Managers.Object.Spawn<Hero>(new Vector3Int(-10 + Random.Range(-5, 5), -5 + Random.Range(-5, 5), 0),  HERO_KNIGHT_ID);
		}

		CameraController camera = Camera.main.GetOrAddComponent<CameraController>();
		camera.Target = camp;

        // ...
```

```csharp
public class HeroCamp : BaseObject
{
	// ...

	private void Update()
	{
		transform.Translate(_moveDir * Time.deltaTime * Speed);
	}

	private void HandleOnMoveDirChanged(Vector2 dir)
	{
		_moveDir = dir;

		if (dir != Vector2.zero)
		{
			float angle = Mathf.Atan2(-dir.x, +dir.y) * 180 / Mathf.PI;
			Pivot.eulerAngles = new Vector3(0, 0, angle);
		}
	}
}

```