---
layout: default
title: "6. Camera"
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

* [Get Code 🌎]()

* C# Script CameraController를 생성하고 아래와 같이 만든다.

```csharp
public class CameraController : MonoBehaviour
{
    [SerializeField]    // Unity에서 지정해 주겠다.
    Define.CameraMode _mode = Define.CameraMode.QuarterView;

    [SerializeField]
    Vector3 _delta = new Vector3(0.0f, 6.0f, -0.5f);

    [SerializeField]
    GameObject _player = null;

    void Start()
    {
        
    }

    // 그냥 Update()를 하지 않는 이유는 캐릭터 이동 후 camera가 이동해야하는데
    // 캐릭터 이동후에 camera의 이동을 강제하기 위해서이다.
    void LateUpdate()
    {
        if (_mode == Define.CameraMode.QuarterView)
        {
            // player를 따라가게 한다.
            transform.position = _player.transform.position + _delta;
            transform.LookAt(_player.transform);
        }
    }
}
```

```csharp
public class Define
{
    public enum CameraMode
    {
        QuarterView,
    }
}
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity/unity-6-1.png"/>
</p>

---

## 마우스, 키보드 컨트롤 변경

```csharp
public class InputManager
{
    public Action KeyAction = null;
    public Action<Define.MouseEvent> MouseAction = null;

    bool _pressed = false;

    public void OnUpdate()
    {
        if (Input.anyKey && KeyAction != null)
            KeyAction.Invoke();

        if (MouseAction != null)
        {
            if (Input.GetMouseButton(0))
            {
                MouseAction.Invoke(Define.MouseEvent.Press);
                _pressed = true;
            }
            else
            {
                if (_pressed)
                    MouseAction.Invoke(Define.MouseEvent.Click);
                _pressed = false;
            }
        }
    }
}
```

```csharp
public class PlayerController : MonoBehaviour
{
    [SerializeField]
    float _speed = 10.0f;

	bool _moveToDest = false;
	Vector3 _destPos;

    void Start()
    {
		Managers.Input.KeyAction -= OnKeyboard;
		Managers.Input.KeyAction += OnKeyboard;
		Managers.Input.MouseAction -= OnMouseClicked;
		Managers.Input.MouseAction += OnMouseClicked;
	}

    void Update()
    {
		if (_moveToDest)
		{
            // 클릭한 위치로 이동
			Vector3 dir = _destPos - transform.position;
			if (dir.magnitude < 0.0001f)
			{
                // 도착
				_moveToDest = false;
			}
			else
			{
                // 이동필요
				float moveDist = Mathf.Clamp(_speed * Time.deltaTime, 0, dir.magnitude);    // min(0) 보다 작다면 min으로, max(dir.magnitube)보다 크다면 max로
				transform.position += dir.normalized * moveDist;
				transform.rotation = Quaternion.Slerp(transform.rotation, Quaternion.LookRotation(dir), 20 * Time.deltaTime);
			}
		}
    }

    void OnKeyboard()
    {
		if (Input.GetKey(KeyCode.W))
		{
			transform.rotation = Quaternion.Slerp(transform.rotation, Quaternion.LookRotation(Vector3.forward), 0.2f);
			transform.position += Vector3.forward * Time.deltaTime * _speed;
		}

		if (Input.GetKey(KeyCode.S))
		{
			transform.rotation = Quaternion.Slerp(transform.rotation, Quaternion.LookRotation(Vector3.back), 0.2f);
			transform.position += Vector3.back * Time.deltaTime * _speed;
		}

		if (Input.GetKey(KeyCode.A))
		{
			transform.rotation = Quaternion.Slerp(transform.rotation, Quaternion.LookRotation(Vector3.left), 0.2f);
			transform.position += Vector3.left * Time.deltaTime * _speed;
		}

		if (Input.GetKey(KeyCode.D))
		{
			transform.rotation = Quaternion.Slerp(transform.rotation, Quaternion.LookRotation(Vector3.right), 0.2f);
			transform.position += Vector3.right * Time.deltaTime * _speed;
		}

		_moveToDest = false;
	}

	void OnMouseClicked(Define.MouseEvent evt)
	{
		if (evt != Define.MouseEvent.Click)
			return;


		Ray ray = Camera.main.ScreenPointToRay(Input.mousePosition);
		Debug.DrawRay(Camera.main.transform.position, ray.direction * 100.0f, Color.red, 1.0f);

		RaycastHit hit;
		if (Physics.Raycast(ray, out hit, 100.0f, LayerMask.GetMask("Wall")))
		{
			_destPos = hit.point;
			_moveToDest = true;
		}
	}
}
```

```csharp
// 카메라 앞에 장애물이 있다면 장애물 앞으로 카메라를 이동
public class CameraController : MonoBehaviour
{
    // ...

    void LateUpdate()
    { 
        if (_mode == Define.CameraMode.QuarterView)
        {
            RaycastHit hit;
            if (Physics.Raycast(_player.transform.position, _delta, out hit, _delta.magnitude, LayerMask.GetMask("Wall")))
            {
                float dist = (hit.point - _player.transform.position).magnitude * 0.8f;
                transform.position = _player.transform.position + _delta.normalized * dist;
            }
            else
            {
				transform.position = _player.transform.position + _delta;
				transform.LookAt(_player.transform);
			}
		}
    }
```