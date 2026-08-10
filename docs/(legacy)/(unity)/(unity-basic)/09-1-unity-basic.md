---
layout: default
title: "9. Scene Manager"
parent: "(Basic)"
grand_parent: "(Unity ✨)"
great_grand_parent: "Legacy Archive"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌍](https://github.com/EasyCoding-7/unity_tutorials/tree/9.1.1)

* 현재는 모든 초기화를 PlayerController에서 하고있는데 매번 Player가 있을꺼라는 보장도 없고 Scene 자체에서 하는게 맞다.
* 그럼 Scene으로 초기화를 할텐데, Scene에서 Scene으로 이동하는 법은?

```csharp
public abstract class BaseScene : MonoBehaviour
{
    public Define.Scene SceneType { get; protected set; } = Define.Scene.Unkown;

    void Start()
    {
        
    }

    protected virtual void Init()
    {

    }

    public abstract void Clear();
}
```

```csharp
public class GameScene : BaseScene
{
    // Start is called before the first frame update
    void Start()
    {
        Init();
    }

    protected override void Init()
    {
        base.Init();

        SceneType = Define.Scene.Game;

        // 이제 UI는 GameScene에서 불러온다.
        Managers.UI.ShowSceneUI<UI_Inven>();
    }

    public override void Clear()
    {

    }
}
```

* 새로운 Scene을 만들어 보자.
* File -> New Scene -> Login Scene 생성
* 새로운 Scene을 만들어 보면 EventSystem이 없는데 이러면 UI 이벤트 처리를 할 수 없다. -> EventSystem을 만들게 변경하자

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity/unity-9-1-1.png"/>
</p>

```csharp
public abstract class BaseScene : MonoBehaviour
{
    // ...

    protected virtual void Init()
    {
        // EvnetSystem을 찾고 없다면 Prefab으로 만들어둔 EventSystem 사용
        Object obj = GameObject.FindObjectOfType(typeof(EventSystem));
        if (obj == null)
            Managers.Resource.Instantiate("UI/EventSystem").name = "@EventSystem";
    }
```

---

* [Get Code 🌍](https://github.com/EasyCoding-7/unity_tutorials/tree/9.1.2)

* LoginScene에서 GameScene으로 넘어가게 해보자.

```csharp
public class UI_Login : BaseScene
{
    public override void Clear()
    {
        
    }

    protected override void Init()
    {
        base.Init();

        SceneType = Define.Scene.Login;
    }

    private void Update()
    {
        if(Input.GetKeyDown(KeyCode.Q))
        {
            // SceneManager는 Unity에서 지원한다.
            SceneManager.LoadScene("Game");
        }
    }
}
```

* 추가적으로 Build Setting도 변경해줘야 된다.
* File -> Build Settings ...

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity/unity-9-1-2.png"/>
</p>

* 여기까지만 해도 Scene이 잘 전환된다.
* 하지만 추가기능을 위해 우리만의 SceneManager를 구현해보자.
* C# Script SceneManagerEx 생성

```csharp
public class SceneManagerEx
{
    public BaseScene CurrentScene { get { return GameObject.FindObjectOfType<BaseScene>(); } }
    public void LoadScene(Define.Scene type)
    {
        CurrentScene.Clear();   // 현재 Scene을 날린다.
        SceneManager.LoadScene(GetSceneName(type));
    }

    string GetSceneName(Define.Scene type)
    {
        string name = System.Enum.GetName(typeof(Define.Scene), type);
        return name;
    }
}
```

```csharp
public class UI_Login : BaseScene
{
    // ...

    private void Update()
    {
        if(Input.GetKeyDown(KeyCode.Q))
        {
            // SceneManager는 Unity에서 지원한다.
            // SceneManager.LoadScene("Game");
            Managers.Scene.LoadScene(Define.Scene.Game);
        }
    }
```