---
layout: default
title: "22. 구역별로 Monster/Map Load/UnLoad"
parent: "(포트폴리오 4 - 방치형 게임)"
grand_parent: "(Unity ✨)"
nav_order: 3
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌎](https://github.com/Arthur880708/Unity.IncrementalGame.Example/tree/9)

---

## Map은 어떻게 구성이 되어있나?

```
Map
|
--- SubMap
|
--- SubMap
...
```

* Map(Base, Dev)에 StageTransition.cs가 붙어있고,
* 하위 SubMap에 Stage.cs가 붙어있다.

---

## Map Load, UnLoad 방법

```csharp
public class StageTransition : InitBase
{
    // ...

    // Map이 로드되며 OnMapChanged 호
    public void SetInfo()
    {
        int currentMapIndex = 0;

        for (int i = 0; i < Stages.Count; i++)
        {
            Stages[i].SetInfo(i);  
            
            if (Stages[i].StartSpawnInfo.WorldPos != Vector3.zero)
            {
                currentMapIndex = i;
            }
        }

        OnMapChanged(currentMapIndex);
	}

    // ..

    public void OnMapChanged(int newMapIndex)
    {
        CurrentStageIndex = newMapIndex;
        CurrentStage = Stages[CurrentStageIndex];
        
        LoadMapsAround(newMapIndex);
        UnloadOtherMaps(newMapIndex);
    }

    // ...

    private void LoadMapsAround(int mapIndex)
    {
        // 주변 맵 까지 로드한다.
        for (int i = mapIndex - 1; i <= mapIndex + 1; i++)
        {
            if (i > -1 && i < Stages.Count) 
            {
                Debug.Log($"{i} Stage Load -> {Stages[i].name}");
                Stages[i].LoadStage();
            }
        }
    }
```

```csharp
public class Stage : MonoBehaviour
{
    // ...

    public void LoadStage()
    {
        if (IsActive)
            return;

        IsActive = true;
        gameObject.SetActive(true);
        // 각 Stage에선 Spawn!
        SpawnObjects();
    }
```


