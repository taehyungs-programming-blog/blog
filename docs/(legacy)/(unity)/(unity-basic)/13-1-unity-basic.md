---
layout: default
title: "13. Data Manager"
parent: "(Basic)"
grand_parent: "(Unity ✨)"
great_grand_parent: "Legacy Archive"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌍](https://github.com/EasyCoding-7/unity_tutorials/tree/13.1)

* 데이터 관련 변수를 별도의 파일로 관리해 보자.

* 그냥 하드코딩해버리면 안되나?

```csharp
class Knight
{
    int hp = 0;
    // 문제)
        // 1) 만약 스펙이 수정돼어 기본 hp가 130이라면 매번 프로그래머가 처리해야함.
        // 2) hp가 수정돼어 긴급 업데이트시 매번 바이너리가 새로 나가야한다.
        // 3) Server와 Client가 같은 데이터 파일을 읽어서 관리하게 해야한다. 안그러면 데이터의 오차가 생길수 있음
    public void Init()
    {
        hp = 120;
    }
}
```

---

## DataManager 구현

```csharp
// [Serializable] - 메모리에 들고있는 것을 파일로 남길수있다(반대도 가능)
[Serializable]
public class Stat
{
    // (주의) 변수 이름 자체는 json과 같아야 한다. 
	public int level;
	public int hp;
	public int attack;
}

[Serializable]
public class StatData : ILoader<int, Stat>  // ILoader???(아래 설명)
{
    // 검색이 없다면 List
	public List<Stat> stats = new List<Stat>();

    // 검색이 필요하다면 Dictionary, id값 Stat값 이렇게 갖고있을 예정
	public Dictionary<int, Stat> MakeDict()
	{
		Dictionary<int, Stat> dict = new Dictionary<int, Stat>();
		foreach (Stat stat in stats)
			dict.Add(stat.level, stat);
		return dict;
	}
}
```

```csharp
// ILoader는 Class의 MakeDict를 강제한다.
public interface ILoader<Key, Value>
{
    Dictionary<Key, Value> MakeDict();
}

public class DataManager
{
    public Dictionary<int, Stat> StatDict { get; private set; } = new Dictionary<int, Stat>();

    public void Init()
    {
        StatDict = LoadJson<StatData, int, Stat>("StatData").MakeDict();
    }

    Loader LoadJson<Loader, Key, Value>(string path) 
    where Loader : ILoader<Key, Value>
    // where은 Loader는 ILoader만 사용되게 해주세요
    {
		TextAsset textAsset = Managers.Resource.Load<TextAsset>($"Data/{path}");
        return JsonUtility.FromJson<Loader>(textAsset.text);
	}
}
```

* 실사용

```csharp
Dictionary<int, Stat> dict = Managers.Data.StatDict;
```