---
layout: default
title: "2. "
title: "Unity-Code-Review"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

🤡 우선 코드위주로 분석 예정

## 시작

```
BaseScene ---- GameScene
            |
            -- DevScene
```

```csharp
public class BaseScene : MonoBehaviour
{
    public Define.Scene SceneType = Define.Scene.Unknown;

    protected bool _init = false;

    private void Start()
    {
        Init();
    }

    protected virtual bool Init()
    {
        if (_init)
            return false;

        _init = true;
        // Event System을 로드
            // EventSystem이 뭔지는 예전강의를 보자.
        GameObject go = GameObject.Find("EventSystem");
        if (go == null)
            Managers.Resource.Instantiate("UI/EventSystem").name = "@EventSystem";

        return true;
    }

    public virtual void Clear() { }
}

```

```csharp
public class GameScene : BaseScene
{
    protected override bool Init()
    {
		if (base.Init() == false)
			return false;

        SceneType = Define.Scene.Game;
		Managers.UI.ShowPopupUI<UI_TitlePopup>();
		Debug.Log("Init");
		return true;
	}
}
```

---

```
UI_Base ----- UI_Popup ----- UI_TitlePopup
```