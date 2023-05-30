---
layout: default
title: "1. 이동처리 + Navigation"
parent: "(포트폴리오 1 - 미니RPG)"
grand_parent: "(Unity ✨)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## 환경설정

* [Get Code 🌍](https://github.com/EasyCoding-7/unity_tutorials/tree/14.1.1)

* 우선 기존의 FrameWork를 활용해서 만들예정이다. (기존의 코드를 지우지 말고 사용하자.)
* 추가 설치할 부분은 Asset Store -> Medieval Town Exteriors
    * 코드에 추가되어 있기에 별도로 할 일은 없다.

---

## 이동처리 (최신 버전)

```csharp
// 현재 상태에 따라 이동을 다르게 처리

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
  // Screen Ray를 구하고
  Ray ray = Camera.main.ScreenPointToRay(Input.mousePosition);

  // Ray가 Masking해 둔 Object에 Hit했는지 체크한다
    // int _mask = (1 << (int)Define.Layer.Ground) | (1 << (int)Define.Layer.Monster);
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

```csharp
	void UpdateMoving()
	{
		// 몬스터가 내 사정거리보다 가까우면 공격
		if (_lockTarget != null)
		{
			_destPos = _lockTarget.transform.position;
			float distance = (_destPos - transform.position).magnitude;
			if (distance <= 1)
			{
				State = PlayerState.Skill;
				return;
			}
		}

		// 이동
		Vector3 dir = _destPos - transform.position;
		if (dir.magnitude < 0.1f)
		{
			State = PlayerState.Idle;
		}
		else
		{
      // NavMeshAgent를 통해 Move동작 확인
        // NavMeshAgent는 아래서 설명
			NavMeshAgent nma = gameObject.GetOrAddComponent<NavMeshAgent>();
			float moveDist = Mathf.Clamp(_stat.MoveSpeed * Time.deltaTime, 0, dir.magnitude);
			nma.Move(dir.normalized * moveDist);

			Debug.DrawRay(transform.position + Vector3.up * 0.5f, dir.normalized, Color.green);
			if (Physics.Raycast(transform.position + Vector3.up * 0.5f, dir, 1.0f, LayerMask.GetMask("Block")))
			{
				if (Input.GetMouseButton(0) == false)
					State = PlayerState.Idle;
				return;
			}

			transform.rotation = Quaternion.Slerp(transform.rotation, Quaternion.LookRotation(dir), 20 * Time.deltaTime);
		}
	}
```

---

## Navigation

* Window -> AI -> Navigation
* 일단은 Bake 클릭

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity/unity-14-1-3.png"/>
</p>

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity/unity-14-1-4.png"/>
</p>

* 파란색이 캐릭터가 걸어다닐수 있는 구역이 된다.
* Navigation Mesh 적용을 위해 Unity-Chan에도 Nav Mesh Agent를 설치해준다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity/unity-14-1-5.png"/>
</p>

* 그런데 풀과 같은 오브젝트도 못가는 영역으로 지정되어 있기에
* Plants의 Static을 끄고 다시 Bake를 해보면 모두 지나갈수 있는 영역이 된 것을 확인할수 있다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity/unity-14-1-6.png"/>
</p>

---

* 그런데 아직도 빌딩을 클릭하면 못가는 영역인데 달리는 모션이 나타나는것을 알수있다.
* 빌딩내에는 갈수있는 영역으로 Bake되었기 때문인데.
* Buildings, Fence, Components 를 Block Layer로 지정하자.

```csharp
void UpdateMoving()
{
    Vector3 dir = _destPos - transform.position;
    if (dir.magnitude < 0.1f)
    {
        _state = PlayerState.Idle;
    }
    else
    {
        NavMeshAgent nva = gameObject.GetOrAddComponent<NavMeshAgent>();
        float moveDist = Mathf.Clamp(_speed * Time.deltaTime, 0, dir.magnitude);
        nva.Move(dir.normalized * moveDist);

        // Block Layer를 만날 경우 Idle로 변경 후 return
          // 사실 이렇게 해도 완벽하게 해결은 안된다. 
          // 더 진행 안하는 이유는 프로젝트마다 달라질수 있기 때문
        Debug.DrawRay(transform.position + Vector3.up * 0.5f, dir.normalized, Color.green);
        if(Physics.Raycast(transform.position + Vector3.up * 0.5f, dir, 1.0f, LayerMask.GetMask("Block")))
        {
            _state = PlayerState.Idle;
            return;
        }

        //transform.position += dir.normalized * moveDist;
        transform.rotation = Quaternion.Slerp(transform.rotation, Quaternion.LookRotation(dir), 20 * Time.deltaTime);
    }
```

---
---

---
---

# 삭제예정

## 이동처리 (이전 버전)

* Player에 PlayerController를 붙여 두었고 아래와 같이 처리 함.

```csharp
public class PlayerController : MonoBehaviour
{
    // ...

	void OnMouseClicked(Define.MouseEvent evt)
	{
		if (_state == PlayerState.Die)
			return;

		Ray ray = Camera.main.ScreenPointToRay(Input.mousePosition);
		Debug.DrawRay(Camera.main.transform.position, // 메인카메라의 방향으로
                        ray.direction * 100.0f,       // mousePosition의 Ray를
                        Color.red,                    // 붉은색으로
                        1.0f);                        // 1초간 그려달라

		RaycastHit hit;

        // (Unity에서 해줘야 할 것) 
            // 이동시 Wall만 Raycast를 받고 있기에 Terrain, Town의 Building을 Wall로 변경해준다.
		if (Physics.Raycast(ray, out hit, 100.0f, LayerMask.GetMask("Wall")))
		{
			_destPos = hit.point;
			_state = PlayerState.Moving;
		}
	}
```

* 이동시 Wall만 Raycast를 받고 있기에 Terrain, Town의 Building을 Wall로 변경해준다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity/unity-14-1-1.png"/>
</p>

* 건물로 이동을 하면 건물을 타버리는데, 해결책이 필요하다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity/unity-14-1-2.png"/>
</p>

* Window -> AI -> Navigation
* 일단은 Bake 클릭

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity/unity-14-1-3.png"/>
</p>

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity/unity-14-1-4.png"/>
</p>

* 파란색이 캐릭터가 걸어다닐수 있는 구역이 된다.
* Navigation Mesh 적용을 위해 Unity-Chan에도 Nav Mesh Agent를 설치해준다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity/unity-14-1-5.png"/>
</p>

* 여기까지만 하면 더이상 벽을 타는 현상은 없어진다.

---

* 추가적으로, 벽을 못타는 대신 벽주변에서 캐릭터가 흔들리는 현상을 잡아보자.

```csharp
void UpdateMoving()
{
    Vector3 dir = _destPos - transform.position;
    if (dir.magnitude < 0.1f)
    {
        _state = PlayerState.Idle;
    }
    else
    {
        // 이동을 NavMeshAgent를 통해서 하게 만들자
        NavMeshAgent nva = gameObject.GetOrAddComponent<NavMeshAgent>();
        // _speed * Time.deltaTime이 min, max를 넘지못하게 한다.
        float moveDist = Mathf.Clamp(_speed * Time.deltaTime, 0/*min*/, dir.magnitude/*max*/);
        nva.Move(dir.normalized * moveDist);

        //transform.position += dir.normalized * moveDist;
        transform.rotation = Quaternion.Slerp(transform.rotation, Quaternion.LookRotation(dir), 20 * Time.deltaTime);
    }

    // ...
```

---

* 그런데 풀과 같은 오브젝트도 못가는 영역으로 지정되어 있기에
* Plants의 Static을 끄고 다시 Bake를 해보면 모두 지나갈수 있는 영역이 된 것을 확인할수 있다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity/unity-14-1-6.png"/>
</p>

---

* 그런데 아직도 빌딩을 클릭하면 못가는 영역인데 달리는 모션이 나타나는것을 알수있다.
* 빌딩내에는 갈수있는 영역으로 Bake되었기 때문인데.
* Buildings, Fence, Components 를 Block Layer로 지정하자.

```csharp
void UpdateMoving()
{
    Vector3 dir = _destPos - transform.position;
    if (dir.magnitude < 0.1f)
    {
        _state = PlayerState.Idle;
    }
    else
    {
        NavMeshAgent nva = gameObject.GetOrAddComponent<NavMeshAgent>();
        float moveDist = Mathf.Clamp(_speed * Time.deltaTime, 0, dir.magnitude);
        nva.Move(dir.normalized * moveDist);

        // Block Layer를 만날 경우 Idle로 변경 후 return
          // 사실 이렇게 해도 완벽하게 해결은 안된다. 
          // 더 진행 안하는 이유는 프로젝트마다 달라질수 있기 때문
        Debug.DrawRay(transform.position + Vector3.up * 0.5f, dir.normalized, Color.green);
        if(Physics.Raycast(transform.position + Vector3.up * 0.5f, dir, 1.0f, LayerMask.GetMask("Block")))
        {
            _state = PlayerState.Idle;
            return;
        }

        //transform.position += dir.normalized * moveDist;
        transform.rotation = Quaternion.Slerp(transform.rotation, Quaternion.LookRotation(dir), 20 * Time.deltaTime);
    }
```

