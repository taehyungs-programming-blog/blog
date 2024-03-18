---
layout: default
title: "23. NPC"
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

## NPC

```csharp
// Npc.cs
    // SetInfo로 데이터를 넣으며 버튼도 같이 넣음

public void SetInfo(int dataId)
{
    Data = Managers.Data.NpcDic[dataId];
    gameObject.name = $"{Data.DataId}_{Data.Name}";

    #region Spine Animation
    SetSpineAnimation(Data.SkeletonDataID, SortingLayers.NPC);
    PlayAnimation(0, AnimName.IDLE, true);
    #endregion

    // Npc 상호작용을 위한 버튼
    GameObject button = Managers.Resource.Instantiate("UI_NpcInteraction", gameObject.transform);
    button.transform.localPosition = new Vector3(0f, 3f);
    // UI_NpcInteraction _ui
    _ui = button.GetComponent<UI_NpcInteraction>();
    _ui.SetInfo(DataTemplateID, this);
}
```

---

## 복습 : UI_Popup 구조

* NPC 내용이 복잡한게 없어서 복습으로 변경

```
                UI_Base
                   |
                   |
                UI_Popup
                   |
                   |
            ----------------
            |      |       |
UI_HeroInfoPopup / UI_HeroesListPopup / UI_WaypointPopup
```

* `UI_Base` - 오히려 얘의 역할이 중요 각종 UI 를 Bind 및 관리담당
    * `UI_Popup` - 큰 역할은 없다 canvas에 UI를 그려준다
        * `UI_HeroInfoPopup` / `UI_HeroesListPopup` / `UI_WaypointPopup` - 그려주는 역할 및 이벤트 핸들링

```csharp
public class UI_HeroesListPopup : UI_Popup
{
	enum GameObjects
	{
		CloseArea,
		EquippedHeroesList,
		WaitingHeroesList,
		UnownedHeroesList,
	}

    // ...

    // 이런식으로 bind
    BindObjects(typeof(GameObjects));

    // ...

    // 이런식으로 쓴
    GetObject((int)GameObjects.CloseArea).BindEvent(OnClickCloseArea);
```
