---
layout: default
title: "7-4. State Machine"
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

* [Get Code 🌍](https://github.com/EasyCoding-7/unity_tutorials/tree/7.4.1)

* 상태관리를 꼭 코드로만 해야하는 것은 아니다.
* Animator 우클릭 -> Make Transition 으로 서로의 상태로 변화를 줄 수 있다.
  * 정확히는 WAIT 우클릭 후 Make Transition클릭, 선이 하나 생기는데 RUN_F에 누르면 된다.
  * 양방향으로 만들어 주자.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity/unity-7-4-1.png"/>
</p>

* 여기까지하면 RUN<->WAIT를 반복하게 된다.
* 이제 특정조건을 넣어 RUN or WAIT을 하게 만들어 보자.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity/unity-7-4-2.png"/>
</p>

* (TIPS) Transitions에 Has Exit Time이 켜져있으면 자동으로 다음 Animation을 동작시키게 된다.

---

* [Get Code 🌍](https://github.com/EasyCoding-7/unity_tutorials/tree/7.4.2)

* 이제 조건을 줘서 상태를 바꿔보자.
* Animator Parameters에 파라미터를 speed(float)로 하나 추가하자

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity/unity-7-4-3.png"/>
</p>

* 각 Transition에 Condition을 추가한다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity/unity-7-4-4.png"/>
</p>

* 아직 speed값을 변경하지 않기에 코드에서 변경해준다.

```csharp
void UpdateMoving()
{
  Vector3 dir = _destPos - transsform.position;
  if(dir.magnitude < 0.0001f>)
  {
    _state = PlayerState.Idle;
  }
  else
  {
    float moveDist = Mathf.Clamp(_speed * Time.deltaTime, 0, dir.magnitude);
    transform.position += dir.normalized * moveDirst;
    transform.rotation = Quaternion.Slerp(transform.rotation, Quaternion.LookRotation(dir), 20 * Time.deltaTime);
  }

  // 애니메이션
  Animator anim = GetComponent<Animator>();

  // 애니메이터컴포넌트에 상수를 넘긴다.
  anim.SetFloat("speed", 0);
}
```