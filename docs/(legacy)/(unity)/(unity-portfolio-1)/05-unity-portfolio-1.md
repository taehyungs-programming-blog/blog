---
layout: default
title: "5. 체력바"
parent: "(포트폴리오 1 - 미니RPG)"
grand_parent: "(Unity ✨)"
great_grand_parent: "Legacy Archive"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌍](https://github.com/EasyCoding-7/unity_tutorials/tree/14.10)

* 우선 Slider를 만들어 적당히 꾸며줘야한다 (이건 코드를 보자)
    * Assets\Prefabs\UI\WorldSpace\UI_HPBar

```csharp
public class PlayerController : BaseController
{
	// ...

	public override void Init()
    {
		WorldObjectType = Define.WorldObject.Player;
		
        // ...

		if (gameObject.GetComponentInChildren<UI_HPBar>() == null)
			Managers.UI.MakeWorldSpaceUI<UI_HPBar>(transform);
```

```csharp
public class UI_HPBar : UI_Base
{
    enum GameObjects
    {
        HPBar
    }

    Stat _stat;

    public override void Init()
    {
        Bind<GameObject>(typeof(GameObjects));
        _stat = transform.parent.GetComponent<Stat>();
    }

    private void Update()
    {
        Transform parent = transform.parent;
        transform.position = parent.position + Vector3.up * (parent.GetComponent<Collider>().bounds.size.y);
        transform.rotation = Camera.main.transform.rotation;

        float ratio = _stat.Hp / (float)_stat.MaxHp;
        SetHpRatio(ratio);
    }

    public void SetHpRatio(float ratio)
    {
        GetObject((int)GameObjects.HPBar).GetComponent<Slider>().value = ratio;
    }
}
```

---

* HP Bar UI는 이렇게 만든다

```csharp
public T MakeWorldSpaceUI<T>(Transform parent = null, string name = null) where T : UI_Base
{
    if (string.IsNullOrEmpty(name))
        name = typeof(T).Name;

    GameObject go = Managers.Resource.Instantiate($"UI/WorldSpace/{name}");
    if (parent != null)
        go.transform.SetParent(parent);

    Canvas canvas = go.GetOrAddComponent<Canvas>();
    canvas.renderMode = RenderMode.WorldSpace;
    canvas.worldCamera = Camera.main;

    return Util.GetOrAddComponent<T>(go);
}
```