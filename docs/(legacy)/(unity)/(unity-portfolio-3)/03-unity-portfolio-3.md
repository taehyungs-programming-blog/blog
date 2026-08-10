---
layout: default
title: "3. Item 생성"
parent: "(포트폴리오 3 - 뱀서 모작)"
grand_parent: "(Unity ✨)"
great_grand_parent: "Legacy Archive"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌎](https://github.com/Arthur880708/Unity_Portfolio_3/tree/3)

* `Sprites\Env\EXPGem_0X` 를 Addressble에 복사한다
* Addressble에 복사하면 Textrue 2D로 생성이 되며 그 아래 Sprite가 생성되는데 이를 기억하자
	* Sprite를 이용해 그릴예정

## Gem Spawn

```csharp
// Gem의 Spawn은 이렇게 처리한다

public T Spawn<T>(Vector3 position, int templateID = 0) where T : BaseController
{
	// ...
	
	else if (type == typeof(GemController))
	{
		GameObject go = Managers.Resource.Instantiate(Define.EXP_GEM_PREFAB, pooling: true);
		go.transform.position = position;

		GemController gc = go.GetOrAddComponent<GemController>();
		Gems.Add(gc);
		gc.Init();

		string key = UnityEngine.Random.Range(0, 2) == 0 ? "EXPGem_01.sprite" : "EXPGem_02.sprite";
		Sprite sprite = Managers.Resource.Load<Sprite>(key);
		go.GetComponent<SpriteRenderer>().sprite = sprite;

		// TEMP
		GameObject.Find("@Grid").GetComponent<GridController>().Add(go);

		return gc as T;
	}

// ...
```

* (참고) Addressble의 preload는 어떻게 동작하나

```csharp
public class GameScene : MonoBehaviour
{
    void Start()
    {
        Managers.Resource.LoadAllAsync<Object>("PreLoad", (key, count, totalCount) =>
        {
            Debug.Log($"{key} {count}/{totalCount}");

			if (count == totalCount)
			{
				StartLoaded();
			}
        });
	}

// ...
```

---

## Gem DeSpawn

```csharp
void CollectEnv()
{
	float sqrCollectDist = EnvCollectDist * EnvCollectDist;

	List<GemController> gems = Managers.Object.Gems.ToList();
	
	var findGems = GameObject.Find("@Grid").GetComponent<GridController>().GatherObjects(transform.position, EnvCollectDist + 0.5f);

	foreach (var go in findGems)
	{
		GemController gem = go.GetComponent<GemController>();

		Vector3 dir = gem.transform.position - transform.position;
		if (dir.sqrMagnitude <= sqrCollectDist)
		{
			Managers.Game.Gem += 1;
			Managers.Object.Despawn(gem);
		}
	}

	Debug.Log($"SearchGems({findGems.Count}) TotalGems({gems.Count})");
}
```
