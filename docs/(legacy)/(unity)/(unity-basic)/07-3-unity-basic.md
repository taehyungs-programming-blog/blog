---
layout: default
title: "7-3. State Pattern 적용"
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

* [Get Code 🌍]()

* 기존 코드의 문제는 if로 상태를 관리하다보니 if의 무한 중첩이 발생할 수 있다.

```csharp
public class PlayerController : MonoBehaviour
{
    //...

	float wait_run_ratio = 0.0f;
	void Update()
	{
		// ...

        // 여기서 새로운 상태가 발생하면 무한정 if를 추가해야한다.
		if (_moveToDest)
		{
			wait_run_ratio = Mathf.Lerp(wait_run_ratio, 1, 10.0f * Time.deltaTime);
			Animator anim = GetComponent<Animator>();
			anim.SetFloat("wiat_run_ratio", wait_run_ratio);
			anim.Play("WAIT_RUN");
		}
        else
        {
			wait_run_ratio = Mathf.Lerp(wait_run_ratio, 0, 10.0f * Time.deltaTime);
			Animator anim = GetComponent<Animator>();
			anim.SetFloat("wiat_run_ratio", wait_run_ratio);
			anim.Play("WAIT_RUN");
		}
	}

    //...
```

* State Pattern을 이용해 해결해보자.

```csharp
public class PlayerController : MonoBehaviour
{
	[SerializeField]
	float _speed = 10.0f;

	Vector3 _destPos;

	void Start()
	{
		Managers.Input.MouseAction -= OnMouseClicked;
		Managers.Input.MouseAction += OnMouseClicked;
	}

	public enum PlayerState
	{
		Die,
		Moving,
		Idle,
		Channeling,
		Jumping,
		Falling
	}

	PlayerState _state = PlayerState.Idle;
	float wait_run_ratio = 0.0f;
	void Update()
	{


		switch (_state)
		{
			case PlayerState.Die:
				UpdateDie();
				break;
			case PlayerState.Moving:
				UpdateMoving();
				break;
			case PlayerState.Idle:
				UpdateIdle();
				break;
		}
	}

	void UpdateDie()
	{
		// 죽었기에 아무것도 안함.
	}

	void UpdateMoving()
    {

		Vector3 dir = _destPos - transform.position;
		if (dir.magnitude < 0.0001f)
		{
			_state = PlayerState.Idle;
		}
		else
		{
			float moveDist = Mathf.Clamp(_speed * Time.deltaTime, 0, dir.magnitude);
			transform.position += dir.normalized * moveDist;
			transform.rotation = Quaternion.Slerp(transform.rotation, Quaternion.LookRotation(dir), 20 * Time.deltaTime);
		}
	
		wait_run_ratio = Mathf.Lerp(wait_run_ratio, 1, 10.0f * Time.deltaTime);
		Animator anim = GetComponent<Animator>();
		anim.SetFloat("wiat_run_ratio", wait_run_ratio);
		anim.Play("WAIT_RUN");
	}

	void UpdateIdle()
    {
		wait_run_ratio = Mathf.Lerp(wait_run_ratio, 0, 10.0f * Time.deltaTime);
		Animator anim = GetComponent<Animator>();
		anim.SetFloat("wiat_run_ratio", wait_run_ratio);
		anim.Play("WAIT_RUN");
	}


	void OnMouseClicked(Define.MouseEvent evt)
	{
		if (_state == PlayerState.Die)
			return;

		Ray ray = Camera.main.ScreenPointToRay(Input.mousePosition);
		Debug.DrawRay(Camera.main.transform.position, ray.direction * 100.0f, Color.red, 1.0f);

		RaycastHit hit;
		if (Physics.Raycast(ray, out hit, 100.0f, LayerMask.GetMask("Wall")))
		{
			_destPos = hit.point;
			_state = PlayerState.Moving;
		}
	}
}
```