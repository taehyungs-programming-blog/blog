---
layout: default
title: "2. 몬스터 준비 + 스탯"
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

* [Get Code 🌍](https://github.com/EasyCoding-7/unity_tutorials/tree/14.5)

## 몬스터 준비

* 몬스터를 준비하자 [Asset Store](https://assetstore.unity.com/packages/3d/characters/humanoids/fantasy-monster-skeleton-35635)에서 이번엔 자신이 원하는 몬스터 아무거나 가져오자
* 추가적으로 이번엔 Animation을 별도로 가져와 보자. [warrior pack bundle 2](https://assetstore.unity.com/packages/3d/animations/warrior-pack-bundle-2-free-42454)

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity/unity-14-2-1.png"/>
</p>

* 모두 필요한건 아니고 Warrior Animation만 있으면 된다.

---

## 스탯관리

* 캐릭터의 스탯을 어떻게 관리하는지 보자.

```csharp
public class Stat : MonoBehaviour
{
    [SerializeField]
    protected int _level;
    [SerializeField]
    protected int _hp;
    [SerializeField]
    protected int _maxHp;
    [SerializeField]
    protected int _attack;
    [SerializeField]
    protected int _defense;
    [SerializeField]
    protected float _moveSpeed;

    public int Level { get { return _level; } set { _level = value; } }
    public int Hp { get { return _hp; } set { _hp = value; } }
    public int MaxHp { get { return _maxHp; } set { _maxHp = value; } }
    public int Attack { get { return _attack; } set { _attack = value; } }
    public int Defense { get { return _defense; } set { _defense = value; } }
    public float MoveSpeed { get { return _moveSpeed; } set { _moveSpeed = value; } }

    private void Start()
    {
        _level = 1;
        _hp = 100;
        _maxHp = 100;
        _attack = 10;
        _defense = 5;
        _moveSpeed = 5.0f;
    }
}
```

```csharp
public class PlayerStat : Stat
{
    [SerializeField]
	protected int _exp;
    [SerializeField]
	protected int _gold;

	public int Exp { get { return _exp; } set { _exp = value; } }
	public int Gold { get { return _gold; } set { _gold = value; } }

	private void Start()
	{
		_level = 1;
		_hp = 100;
		_maxHp = 100;
		_attack = 10;
		_defense = 5;
		_moveSpeed = 5.0f;
		_exp = 0;
		_gold = 0;
	}
}
```

* Monster와 Player에 각각 붙이자.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity/unity-14-2-2.png"/>
</p>
