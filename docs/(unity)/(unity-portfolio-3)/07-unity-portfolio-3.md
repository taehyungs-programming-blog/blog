---
layout: default
title: "7. UI"
parent: "(포트폴리오 3 - 뱀서 모작)"
grand_parent: "(Unity ✨)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* 다른 부분은 크게 어렵지 않고, 생각해 봐야할 부분은 Gem(Item)획득시 UI처리이다.
* 단순하게 생각하며 Player가 먹으니 Player에서 처리하면 되지 않을까 싶지만
* 그럴경우 Player가 UIManager에 직접접근해야하며 이후 버그의 소지가 될수 있다.

* 절대절대 주의사항은 UI에 절대절대 데이터를 넣어선 안된다는 것이다.
* 데이터는 별도로 관리해야한다

```csharp
// GameScene(중앙)에서 Gem을 관리

void StartLoaded()
{
    // ...

    Managers.Game.OnKillCountChanged -= HandleOnKillCountChanged;
    Managers.Game.OnKillCountChanged += HandleOnKillCountChanged;
    Managers.Game.OnGemCountChanged -= HandleOnGemCountChanged;
    Managers.Game.OnGemCountChanged += HandleOnGemCountChanged;
}

int _collectedGemCount = 0;
int _remainingTotalGemCount = 10;

public void HandleOnGemCountChanged(int gemCount)
{
    _collectedGemCount++;

    if (_collectedGemCount == _remainingTotalGemCount)
    {
        Managers.UI.ShowPopup<UI_SkillSelectPopup>();
        _collectedGemCount = 0;
        _remainingTotalGemCount *= 2;
    }

    // Gem의 변화가 생길시 UI Update
    Managers.UI.GetSceneUI<UI_GameScene>().SetGemCountRatio((float)_collectedGemCount / _remainingTotalGemCount);
}
```

```csharp
// Player에서는

void CollectEnv()
{
    float sqrCollectDist = EnvCollectDist * EnvCollectDist;
    
    var findGems = GameObject.Find("@Grid").GetComponent<GridController>().GatherObjects(transform.position, EnvCollectDist + 0.5f);

    foreach (var go in findGems)
    {
        GemController gem = go.GetComponent<GemController>();

        Vector3 dir = gem.transform.position - transform.position;
        if (dir.sqrMagnitude <= sqrCollectDist)
        {
            // Manager에서 관리중인 Gem에 접근해서 숫자만 변경
            Managers.Game.Gem += 1;

            Managers.Object.Despawn(gem);
        }
    }
}
```