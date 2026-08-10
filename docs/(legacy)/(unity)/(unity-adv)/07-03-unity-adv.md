---
layout: default
title: "7.3. 소멸 이펙트"
parent: "(Advanced)"
grand_parent: "(Unity ✨)"
great_grand_parent: "Legacy Archive"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌍](https://github.com/EasyCoding-7/UnityPortfolio/tree/7.3.Destroy)

---

* 몬스터 사망시 나타나는 애니메이션이 필요하다
* **Tiny RPG Forest/Artwork/sprites/misc/enemy-death**의 sprite중 하나를 gameobject에 넣으면 Sprite Renderer가 붙은 Gameobject가 하나 생성된다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity-adv/unity-adv-7-3-1.png"/>
</p>

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity-adv/unity-adv-7-3-2.png"/>
</p>

* 생성한 DieEffect GameObject에 Animator 컴포넌트를 하나 추가하고, Animator Controller를 추가한다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity-adv/unity-adv-7-3-3.png"/>
</p>

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity-adv/unity-adv-7-3-4.png"/>
</p>

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity-adv/unity-adv-7-3-5.png"/>
</p>

```csharp
public class MonsterController : CreatureController
{
    // ...

	public override void OnDamanaged()
	{
		GameObject effect = Managers.Resource.Instantiate("Effect/DieEffect");
		effect.transform.position = transform.position;
		effect.GetComponent<Animator>().Play("START");

		GameObject.Destroy(effect, 0.5f);
		Managers.Object.Remove(gameObject);
		Managers.Resource.Destroy(gameObject);
	}
```

```csharp
public class ArrowController : CreatureController
{
    // ...

	protected override void UpdateIdle()
	{
		// ...

			State = CreatureState.Moving;
			if (Managers.Map.CanGo(destPos))
			{
				GameObject go = Managers.Object.Find(destPos);
				if (go == null)
				{
					CellPos = destPos;
				}
				else
                {
					CreatureController cc = go.GetComponent<CreatureController>();
					if(cc != null)
						cc.OnDamanaged();

					Debug.Log(go.name);
					Managers.Resource.Destroy(gameObject);
				}
			}
```