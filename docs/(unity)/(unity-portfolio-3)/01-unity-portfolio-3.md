---
layout: default
title: "1. 기본 틀 + Joystick 구현"
parent: "(포트폴리오 3 - 뱀서 모작)"
grand_parent: "(Unity ✨)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌎](https://github.com/Arthur880708/Unity_Portfolio_3/tree/1)

## (TIPS) 여러 이미지 파일로 .anim 애니메이션 파일만들기

* 그냥 드래그앤 드롭해서 Scene에 넣으면 .anim으로 Unity가 만들어준다

---

## GameScene

```csharp
public class GameScene : MonoBehaviour
{
    // 현재는 일종의 Manager로 사용하고 있다
    public GameObject _snakePrefab;
    public GameObject _slimePrefab;
    public GameObject _goblinPrefab;
    public GameObject _joystickPrefab;

    GameObject _snake;
    GameObject _slime;
    GameObject _goblin;
    GameObject _joystick;

    void Start()
    {
        // prefab으로 instance를 생성한다
            // prefab은 editor에서 연결해 둠
		_snake = GameObject.Instantiate(_snakePrefab);
        _slime = GameObject.Instantiate(_slimePrefab);
        _goblin = GameObject.Instantiate(_goblinPrefab);
		_joystick = GameObject.Instantiate(_joystickPrefab);

        // 각 parent로 Scene을 연결
        GameObject go = new GameObject() { name = "@Monsters" };
        _snake.transform.parent = go.transform;
		//_slime.transform.parent = go.transform;
		_goblin.transform.parent = go.transform;

		_snake.name = _snakePrefab.name;
		_slime.name = _slimePrefab.name;
		_goblin.name = _goblinPrefab.name;

        _slime.AddComponent<PlayerController>();

        // 카메라를 slime(주인공에 연결
        Camera.main.GetComponent<CameraController>().Target = _slime;

        _joystick.name = "@UI_Joystick";

	}

    void Update()
    {
        
    }
}
```

---

## Camera

```csharp
public class CameraController : MonoBehaviour
{
    public GameObject Target;

    void Start()
    {
    }

    void LateUpdate()
    {
        if (Target == null)
            return;

        transform.position = new Vector3(Target.transform.position.x, Target.transform.position.y, -10);
    }
}
```

* 여기서 궁금한건 카메라를 안달았는데 어디서 카메라를 찍느냐?
* Main Camera Component에 Editor에서 붙임

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unity/portfolio-3/p3-1-1.png"/>
</p>

---

## Joystick

* Joystick의 주의사항은 Prefab에서 Raycast Target을 지정해놔야 우선순위로 터치 이벤트를 처리할 수 있다

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unity/portfolio-3/p3-1-2.png"/>
</p>

```csharp
public class UI_Joystick : MonoBehaviour, IPointerClickHandler, IPointerDownHandler, IPointerUpHandler, IDragHandler
{
    [SerializeField]
    Image _background;

	[SerializeField]
	Image _handler;

	float _joystickRadius;
	Vector2 _touchPosition;
	Vector2 _moveDir;

	PlayerController _player;

    void Start()
    {
		_joystickRadius = _background.gameObject.GetComponent<RectTransform>().sizeDelta.y / 2;
		_player = GameObject.Find("Slime_01").GetComponent<PlayerController>();
	}

    // Update is called once per frame
    void Update()
    {
        
    }

	public void OnPointerClick(UnityEngine.EventSystems.PointerEventData eventData)
	{
		//Debug.Log("OnPointerClick");

		
	}

	public void OnPointerDown(UnityEngine.EventSystems.PointerEventData eventData)
	{
		//Debug.Log("OnPointerDown");
		_background.transform.position = eventData.position;
		_handler.transform.position = eventData.position;
		_touchPosition = eventData.position;
	}

	public void OnPointerUp(UnityEngine.EventSystems.PointerEventData eventData)
	{
		//Debug.Log("OnPointerUp");
		_handler.transform.position = _touchPosition;
		_moveDir = Vector2.zero;

		// TEMP1
		_player.MoveDir = _moveDir;
		// TEMP2
		Managers.MoveDir = _moveDir;
	}

	public void OnDrag(UnityEngine.EventSystems.PointerEventData eventData)
	{
		//Debug.Log("OnDrag");
		Vector2 touchDir = (eventData.position - _touchPosition);

		float moveDist = Mathf.Min(touchDir.magnitude, _joystickRadius);
		_moveDir = touchDir.normalized;
		Vector2 newPosition = _touchPosition + _moveDir * moveDist;
		_handler.transform.position = newPosition;

		// TEMP1
		//_player.MoveDir = _moveDir;

		// TEMP2
		Managers.MoveDir = _moveDir;
	}
}
```