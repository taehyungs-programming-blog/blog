---
layout: default
title: "5. Controller 정리"
parent: "(Advanced)"
grand_parent: "(Unity ✨)"
great_grand_parent: "Legacy Archive"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌍](https://github.com/EasyCoding-7/UnityPortfolio/tree/5.Controller)

* 몬스터용으로 사용할 Player를 하나 복사해 색상만 바꿔 보자.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity-adv/unity-adv-5-1.png"/>
</p>

* 이제 몬스터용 컨트롤러(MonsterController.cs)를 별도로 만들어보자.

```csharp
public class MonsterController : CreatureController
{
	protected override void Init()
	{
		base.Init();
	}

	protected override void UpdateController()
	{
		//GetDirInput();
		base.UpdateController();
	}

	// 키보드 입력
	void GetDirInput()
	{
		if (Input.GetKey(KeyCode.W))
		{
			Dir = MoveDir.Up;
		}
		else if (Input.GetKey(KeyCode.S))
		{
			Dir = MoveDir.Down;
		}
		else if (Input.GetKey(KeyCode.A))
		{
			Dir = MoveDir.Left;
		}
		else if (Input.GetKey(KeyCode.D))
		{
			Dir = MoveDir.Right;
		}
		else
		{
			Dir = MoveDir.None;
		}
	}
}
```

```csharp
// 추가적으로 될수 있으면 상태관리를 State를 둬서 하지 bool을 쓰지는 말자.

void UpdatePosition()
{
    if (State != CreatureState.Moving)
        return;

    Vector3 destPos = Managers.Map.CurrentGrid.CellToWorld(_cellPos) + new Vector3(0.5f, 0.5f);
    Vector3 moveDir = destPos - transform.position;

    // 도착 여부 체크
    float dist = moveDir.magnitude;
    if (dist < _speed * Time.deltaTime)
    {
        transform.position = destPos;
        // 예외적으로 애니메이션을 직접 컨트롤
        // 상태 감시 후 동작여부를 체크
        _state = CreatureState.Idle;
        if (_dir == MoveDir.None)
            UpdateAnimation();
    }
    else
    {
        transform.position += moveDir.normalized * _speed * Time.deltaTime;
        State = CreatureState.Moving;
    }
}
```

```csharp
// 상태에 따라서 애니메이션 처리

protected virtual void UpdateAnimation()
{
    if (_state == CreatureState.Idle)
    {
        switch (_lastDir)
        {
            case MoveDir.Up:
                _animator.Play("IDLE_BACK");
                _sprite.flipX = false;
                break;

    // ...
```
