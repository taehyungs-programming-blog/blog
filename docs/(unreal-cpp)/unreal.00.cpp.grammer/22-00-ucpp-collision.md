---
layout: default
title: "22. 충돌 기초"
parent: "(문법)"
grand_parent: "(Unreal C++ 🚀)"
nav_order: 3
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌟](https://github.com/Arthur880708/UnrealEngineGrammer/tree/2)

---

## Collsion 세팅

* 우선 BP로 하나의 Box를 만들어보자
* 다음은 Collsion을 하나 붙인다

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/grammer/ucpp0-22-1.png"/>
</p>

* 이제 Detail에서 Collsion탭을 살펴보자 공부하는 수 밖에...

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/grammer/ucpp0-22-2.png"/>
</p>

---

## Preset

* Project Settings -> Engine -> Collision

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/grammer/ucpp0-22-3.png"/>
</p>

* Object Channel(물체)를 추가해 보자

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/grammer/ucpp0-22-4.png"/>
</p>

* BP도 추가됨을 확인

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/grammer/ucpp0-22-5.png"/>
</p>

* Trace Channel - 물채는 아님, 예를들어 Camera같은 애들의 충돌을 어떻게 처리할 것이냐?

---

## C++에서 처리

```cpp
AR1Player::AR1Player()
{
	// ...

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	//GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
}

void AR1Player::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Log, TEXT("OnBeginOverlap"));
}
```
