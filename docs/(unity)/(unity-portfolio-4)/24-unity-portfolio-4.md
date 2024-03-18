---
layout: default
title: "24. UI 최적화"
parent: "(포트폴리오 4 - 방치형 게임)"
grand_parent: "(Unity ✨)"
nav_order: 3
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌎](https://github.com/Arthur880708/Unity.IncrementalGame.Example/tree/10)

---

```csharp
public void CacheAllPopups()
{
    var list = AppDomain.CurrentDomain.GetAssemblies()
        .SelectMany(assembly => assembly.GetTypes())
        .Where(type => type.IsSubclassOf(typeof(UI_Popup)));

    foreach (Type type in list)
    {
        CachePopupUI(type);
    }

    // ShowPopupUI<UI_WaypointPopup>();
    
    CloseAllPopupUI();
}
```

* `AppDomain.CurrentDomain.GetAssemblies()` - 현재 C#에 CurrentDomain에 올라온 모든 어샘블리를 list로 가져와 달라
    * 사실 정확히 의미는 모르겠음 ..
* `.SelectMany(assembly => assembly.GetTypes())` - 가져온 어샘블리 list의 type정보를 가져와 달라
* `.Where(type => type.IsSubclassOf(typeof(UI_Popup)));` - type정보 중 UI_Popup만 가져와 달라


```csharp
public void CachePopupUI(Type type)
{
    string name = type.Name;

    if (_popups.TryGetValue(name, out UI_Popup popup) == false)
    {
        GameObject go = Managers.Resource.Instantiate(name);
        popup = go.GetComponent<UI_Popup>();
        // 이렇게 미리 담아둔다
        _popups[name] = popup;
    }

    _popupStack.Push(popup);
}
```