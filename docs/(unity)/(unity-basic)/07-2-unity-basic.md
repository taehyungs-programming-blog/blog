---
layout: default
title: "7-2. Animation Blending"
parent: "Link Page"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* 부드럽게 서고, 뛰게 만들어보자.

## 코드에서 Animator 적용해보기

* [Get Code 🌍]()

```csharp
public class PlayerController : MonoBehaviour
{
    //...

	void Update()
	{
		if (_moveToDest)
		{
			Vector3 dir = _destPos - transform.position;
			if (dir.magnitude < 0.0001f)
			{
				_moveToDest = false;
			}
			else
			{
				float moveDist = Mathf.Clamp(_speed * Time.deltaTime, 0, dir.magnitude);
				transform.position += dir.normalized * moveDist;
				transform.rotation = Quaternion.Slerp(transform.rotation, Quaternion.LookRotation(dir), 20 * Time.deltaTime);
			}
		}

		if (_moveToDest)
		{
			Animator anim = GetComponent<Animator>();
			anim.Play("RUN_F");
		}
        else
        {
			Animator anim = GetComponent<Animator>();
			anim.Play("WAIT");
        }
	}

    //...
```

---

## Animation Blending

* [Get Code 🌍]()

* 움직임 시작과 끝에 끊어지는 듯한 모습이 그리 보기 좋지 못하다 Blend를 이용해 해결해 보자.
* Animator 우클릭 -> Create State -> From New Blend Tree
* 새로 생성한 Blend Tree의 이름을 WAIT_RUN으로 변경 후 더블클릭해보자.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity/unity-7-2-1.png"/>
</p>

* 우클릭 -> Add Motion 두 번
* 아래와 같이 변경해 주자.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity/unity-7-2-2.png"/>
</p>

* Animator의 파라미터를 float으로 하나 추가한다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity/unity-7-2-3.png"/>
</p>

* WAIT_RUN Blend Tree에서 사용하는 파라미터를 wiat_run_ratio로 바꿔준다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity/unity-7-2-4.png"/>
</p>

```csharp
public class PlayerController : MonoBehaviour
{
    //...

	float wait_run_ratio = 0.0f;
	void Update()
	{
		// ...

		if (_moveToDest)
		{
			// 0 -> 1로 천천히 이동
			wait_run_ratio = Mathf.Lerp(wait_run_ratio, 1, 10.0f * Time.deltaTime);
			Animator anim = GetComponent<Animator>();
			anim.SetFloat("wiat_run_ratio", wait_run_ratio);
			anim.Play("WAIT_RUN");
		}
        else
        {
			// 1 -> 0로 천천히 이동
			wait_run_ratio = Mathf.Lerp(wait_run_ratio, 0, 10.0f * Time.deltaTime);
			Animator anim = GetComponent<Animator>();
			anim.SetFloat("wiat_run_ratio", wait_run_ratio);
			anim.Play("WAIT_RUN");
		}
	}

    //...
```