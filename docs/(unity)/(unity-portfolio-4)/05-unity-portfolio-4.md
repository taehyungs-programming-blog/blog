---
layout: default
title: "05. JoyStick 붙이기"
parent: "(포트폴리오 4 - 방치형 게임)"
grand_parent: "(Unity ✨)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## 우선, 각 Object의 로딩

```csharp
public class GameScene : BaseScene
{
	public override bool Init()
	{
		if (base.Init() == false)
			return false;

		SceneType = EScene.GameScene;

        // Map Load
		GameObject map = Managers.Resource.Instantiate("BaseMap");
		map.transform.position = Vector3.zero;
		map.name = "@BaseMap";

        // Hero Load
            // Managers.Object에서 Spawn 하며 각종 처리를 함(Pooling 등)
		Hero hero = Managers.Object.Spawn<Hero>(new Vector3Int(-10, -5, 0));
		hero.CreatureState = ECreatureState.Move;

        // Camera Load
            // Unity Camera Object에 CameraController를 붙임을 주목.
		CameraController camera = Camera.main.GetOrAddComponent<CameraController>();
		camera.Target = hero;

        // JoyStick Load
		Managers.UI.ShowBaseUI<UI_Joystick>();

		{
			Monster monster = Managers.Object.Spawn<Monster>(new Vector3Int(0, 1, 0));
			monster.CreatureState = ECreatureState.Idle;
		}

		return true;
	}
```

---

## JoyStick은 어떻게 동작할까?

```csharp
// ...

public class UI_Joystick : UI_Base
{
	// ...

	public override bool Init()
	{
		// ...

        // Event Bind
		gameObject.BindEvent(OnPointerDown, type: Define.EUIEvent.PointerDown);
		gameObject.BindEvent(OnPointerUp, type: Define.EUIEvent.PointerUp);
		gameObject.BindEvent(OnDrag, type: Define.EUIEvent.Drag);

		return true;
	}

	#region Event
	public void OnPointerDown(PointerEventData eventData)
	{
		_background.transform.position = eventData.position;
		_cursor.transform.position = eventData.position;
		_touchPos = eventData.position;

		Managers.Game.JoystickState = EJoystickState.PointerDown;
	}

	public void OnPointerUp(PointerEventData eventData)
	{
		_cursor.transform.position = _touchPos;

		Managers.Game.MoveDir = Vector2.zero;
		Managers.Game.JoystickState = EJoystickState.PointerUp;
	}

	public void OnDrag(PointerEventData eventData)
	{
		Vector2 touchDir = (eventData.position - _touchPos);

		float moveDist = Mathf.Min(touchDir.magnitude, _radius);
		Vector2 moveDir = touchDir.normalized;
		Vector2 newPosition = _touchPos + moveDir * moveDist;
		_cursor.transform.position = newPosition;

		Managers.Game.MoveDir = moveDir;
		Managers.Game.JoystickState = EJoystickState.Drag;
	}
	#endregion
}
```

* 결국 `Managers.Game`로 뭔가 데이터를 넘겨준다

```csharp
// ...

public class Hero : Creature
{
	Vector2 _moveDir = Vector2.zero;

	public override bool Init()
	{
		// ...

        // Hero에서 모두 등록해 둠.
		Managers.Game.OnMoveDirChanged -= HandleOnMoveDirChanged;
		Managers.Game.OnMoveDirChanged += HandleOnMoveDirChanged;
		Managers.Game.OnJoystickStateChanged -= HandleOnJoystickStateChanged;
		Managers.Game.OnJoystickStateChanged += HandleOnJoystickStateChanged;

		return true;
	}

	// ...
```