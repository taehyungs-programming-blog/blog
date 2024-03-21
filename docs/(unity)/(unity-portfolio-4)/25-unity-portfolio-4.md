---
layout: default
title: "25. 🐧 중간정리 🐧"
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

* 중점적으로 볼 사항은
    * **어떻게** 구현 했는가?
    * 왜 이렇게 **설계** 했는가?

## 클릭 시 첫 화면 넘어가는 부분

* 정확히는 Bind, Get 시스템의 이해

### 어떻게 구현?

```csharp
public abstract class BaseScene : InitBase
{
	public EScene SceneType { get; protected set; } = EScene.Unknown;

	public override bool Init()
	{
		if (base.Init() == false)
			return false;

		Object obj = GameObject.FindObjectOfType(typeof(EventSystem));
		if (obj == null)
		{
			GameObject go = new GameObject() { name = "@EventSystem" };
            // 우선, 입력처리를 위해 EventSystem와 StandaloneInputModule를 붙인다.
			go.AddComponent<EventSystem>();
			go.AddComponent<StandaloneInputModule>();
		}

		return true;
	}

    // ...
```

```csharp
public class UI_TitleScene : UI_Scene
{
    enum GameObjects
    {
        StartImage
    }

    // ...

    public override bool Init()
    {
        if (base.Init() == false)
            return false;

        BindObjects(typeof(GameObjects));
        BindTexts(typeof(Texts));

        // StartImage의 ClickEvent를 아래와 같이 걸어둔다.
		GetObject((int)GameObjects.StartImage).BindEvent((evt) =>
		{
			Debug.Log("ChangeScene");
			Managers.Scene.LoadScene(EScene.GameScene);
		});

		// ...
```

### 왜 이렇게 설계?

* C#의 Reflection의 활용이 핵심이다.

```csharp
// UI_Base.cs - Bind

// type으로 enum이 넘어온다
protected void Bind<T>(Type type) where T : UnityEngine.Object
{
    // enum의 배열의 요소 이름이 names로 들어감
        // 런타임에 enum의 이름을 알 수 있음.
    string[] names = Enum.GetNames(type);
    UnityEngine.Object[] objects = new UnityEngine.Object[names.Length];
    _objects.Add(typeof(T), objects);

    for (int i = 0; i < names.Length; i++)
    {
        // object를 찾아 배열에 넣어둔
        if (typeof(T) == typeof(GameObject))
            objects[i] = Util.FindChild(gameObject, names[i], true);
        else
            objects[i] = Util.FindChild<T>(gameObject, names[i], true);

        if (objects[i] == null)
            Debug.Log($"Failed to bind({names[i]})");
    }
}
```

```csharp
// UI_Base.cs - Get

// index는 enum을 int로 형 변환시 index가 된다
protected T Get<T>(int idx) where T : UnityEngine.Object
{
    UnityEngine.Object[] objects = null;
    if (_objects.TryGetValue(typeof(T), out objects) == false)
        return null;

    return objects[idx] as T;
}
```

---

## UI, HeroList

* UI Prefab에 Script를 붙이는 식으로 구현된다.
* 중요한건 UI에 데이터 주입을 어떻게 하는가?

### 어떻게 구현?

```csharp
// UI_HerosListPopup.cs

void Refresh_Hero(List<UI_HeroesList_HeroItem> list, HeroOwningState owningState)
{
    List<HeroSaveData> heroes = Managers.Game.AllHeroes.Where(h => h.OwningState == owningState).ToList();

    for (int i = 0; i < list.Count; i++)
    {
        if (i < heroes.Count)
        {
            HeroSaveData hero = heroes[i];
            // 여기서 list_item으로 Info index를 보낸
            list[i].SetInfo(hero.DataId);
            list[i].gameObject.SetActive(true);
        }
        else
        {
            list[i].gameObject.SetActive(false);
        }
    }
}
```

```csharp
// UI_HeroesList_HeroItem.cs

void Refresh()
{
    if (_init == false)
        return;

    if (_heroDataId < 0)
        return;

    // 여기서 Resource를 읽어 쓴다
        // 핵심은 UI(View)에서 어떠한 데이터 가공도 없이 그저 읽기만
    GetImage((int)Images.HeroImage).sprite = Managers.Resource.Load<Sprite>(Managers.Data.HeroDic[_heroDataId].IconImage);
}
```

### 왜 이렇게 설계?

* View와 Controller의 구분을 명확히 하자!

---

## 캐릭터 이동

### 어떻게 구현?

```csharp
// UI_Joystick.cs

// Bind를 걸고
gameObject.BindEvent(OnPointerDown, type: Define.EUIEvent.PointerDown);

// ...

// Event가 동작시
public void OnPointerDown(PointerEventData eventData)
{
    _touchPos = Input.mousePosition;

    Vector2 mouseWorldPos = Camera.main.ScreenToWorldPoint(Input.mousePosition);
    _background.transform.position = mouseWorldPos;
    _cursor.transform.position = mouseWorldPos;

    // JoysticState를 수정한다
    Managers.Game.JoystickState = EJoystickState.PointerDown;
}

// ...

// JoystickState은 아래와 같이 set할시 이벤트 발동
public Define.EJoystickState JoystickState
{
    get { return _joystickState; }
    set
    {
        _joystickState = value;
        OnJoystickStateChanged?.Invoke(_joystickState);
    }
}
```

```csharp
// Hero.cs

// State를 업데이트 한다
private void HandleOnJoystickStateChanged(EJoystickState joystickState)
{
    switch (joystickState)
    {
        case Define.EJoystickState.PointerDown:
            HeroMoveState = EHeroMoveState.ForceMove;
            break;
        case Define.EJoystickState.Drag:
            HeroMoveState = EHeroMoveState.ForceMove;
            break;
        case Define.EJoystickState.PointerUp:
            HeroMoveState = EHeroMoveState.None;
            break;
        default:
            break;
    }
}
```

```csharp
// HeroCamp.cs

    // 내부적으로 위치가 관리 됨.
private void Update()
{
    Vector3 dir = _moveDir * Time.deltaTime * Speed;
    Vector3 newPos = transform.position + dir;

    if (Managers.Map == null)
        return;
    if (Managers.Map.CanGo(null, newPos, ignoreObjects: true, ignoreSemiWall: true) == false)
        return;

    transform.position = newPos;
    Managers.Map.StageTransition.CheckMapChanged(newPos);
}
```

### 왜 이렇게 설계?

* 1. UI_Joystick에서 발생한 Event를 GameManager에서 관리
    * 이건 타당.
* 2. GameManager에서 받은 Joystick-Event를 토대로 HeroMoveState 변경
    * State만 확인 후 동작은 각 Creature가 알아서 처리함.
* 3. 어디로 이동할지 여부 데이터도 HeroCamp에서 담당
    * 역할의 분리

---

## UI, WayPoint 및 이동

### 어떻게 구현?

### 왜 이렇게 설계?

---

## 전투시스템

### 어떻게 구현?

### 왜 이렇게 설계?

---

## Stat 시스템

### 어떻게 구현?

### 왜 이렇게 설계?