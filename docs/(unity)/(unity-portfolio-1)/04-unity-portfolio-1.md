---
layout: default
title: "4. 몬스터 락온"
parent: "(포트폴리오 1 - 미니RPG)"
grand_parent: "(Unity ✨)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌍](https://github.com/EasyCoding-7/unity_tutorials/tree/14.5)

* **락온?** - 몬스터를 클릭 후 마우스를 누르고 있으면 계속 공격
* 땅을 클릭 후 마우스를 누르고 있으면 이동, 이 두가지를 나누려한다.

```csharp
public class InputManager
{
    public Action KeyAction = null;
    public Action<Define.MouseEvent> MouseAction = null;

    bool _pressed = false;
    float _pressedTime = 0;

    public void OnUpdate()
    {
        if (EventSystem.current.IsPointerOverGameObject())
            return;

        if (Input.anyKey && KeyAction != null)
				KeyAction.Invoke();

        if (MouseAction != null)
        {
            if (Input.GetMouseButton(0))
            {
                if (!_pressed)
                {
                  // 처음 눌릴시
                    MouseAction.Invoke(Define.MouseEvent.PointerDown);
                    _pressedTime = Time.time;
                }
                // 누르고 있는 동안
                MouseAction.Invoke(Define.MouseEvent.Press);
                _pressed = true;
            }
            else
            {
                if (_pressed)
                {
                    if (Time.time < _pressedTime + 0.2f)
                      // 짧은 시간 누를시 클릭 처리
                        MouseAction.Invoke(Define.MouseEvent.Click);
                    // 긴 시간 누를시 up처리
                    MouseAction.Invoke(Define.MouseEvent.PointerUp);
                }
                _pressed = false;
                _pressedTime = 0;
            }
        }
    }

    public void Clear()
    {
        KeyAction = null;
        MouseAction = null;
    }
}
```

```csharp
void OnMouseEvent(Define.MouseEvent evt)
{
  switch (State)
  {
    case PlayerState.Idle:
      OnMouseEvent_IdleRun(evt);
      break;
    case PlayerState.Moving:
      OnMouseEvent_IdleRun(evt);
      break;
    case PlayerState.Skill:
      {
        // 스킬이 사용중에 마우스 업을 받으면 중지 처리
        if (evt == Define.MouseEvent.PointerUp)
          _stopSkill = true;
      }
      break;
  }
}
```

```csharp
void OnMouseEvent_IdleRun(Define.MouseEvent evt)
{
  RaycastHit hit;
  Ray ray = Camera.main.ScreenPointToRay(Input.mousePosition);
  bool raycastHit = Physics.Raycast(ray, out hit, 100.0f, _mask);
  //Debug.DrawRay(Camera.main.transform.position, ray.direction * 100.0f, Color.red, 1.0f);

  switch (evt)
  {
    case Define.MouseEvent.PointerDown:
      {
        if (raycastHit)
        {
          _destPos = hit.point;
          State = PlayerState.Moving;
          _stopSkill = false;

          // 몬스터 만날시 처리.
          if (hit.collider.gameObject.layer == (int)Define.Layer.Monster)
            _lockTarget = hit.collider.gameObject;
          else
            _lockTarget = null;
        }
      }
      break;
    case Define.MouseEvent.Press:
      {
        if (_lockTarget == null && raycastHit)
          _destPos = hit.point;
      }
      break;
    case Define.MouseEvent.PointerUp:
      _stopSkill = true;
      break;
  }
}
```