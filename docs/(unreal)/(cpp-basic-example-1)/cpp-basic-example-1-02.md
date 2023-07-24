---
layout: default
title: "02. Unreal C++ 다뤄보기"
parent: "(Unreal C++ - Basic Example)"
grand_parent: "(Unreal 🚀)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## UPROPERTY

* 아래와 같이 수정 후 Editor에서 Recompile And Reload버튼을 클릭

<사진1>

```cpp
UCLASS()
class UNREALEXAMPLE01_API AMovingActors : public AActor
{
	GENERATED_BODY()
	
    // ...

	UPROPERTY(EditAnywhere)
	int32 MyInt = 99;

	UPROPERTY(EditAnywhere)
	int32 MyInt2 = 999;
};
```

---

## Actor이동시켜보기

```cpp
void AMovingActors::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector v = GetActorLocation();
	v.Z = v.Z + 1;

	SetActorLocation(v);
}
```

* 추가) 속도를 이용해 이동시켜보기

```cpp
void AMovingActors::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector v = GetActorLocation();
	v = v + PlatformVelocity * DeltaTime;

	SetActorLocation(v);
}
```

---

## C++클래스를 Blueprint클래스로 만들어보자

* C++ 클래스 우클릭 후 Create Blueprint class based on ... 클릭

<사진>

---

## 충돌처리

* 현재의 문제점은 충돌이 되면 카메라 처리가 이상하게 된다. + 캐릭터가 밀리지 않는다

<사진>

* 해결법은 일종의 편법을 사용해야한다
* MoveUpdate를 호출하여 캐릭터의 움직임의 변화를 매 Tick체크해야함

<사진>

---

## 캐릭터 시작점 설정하기

* 우선 아래와 같이 GameMode를 생성해보자

<사진>

* 생성된 GameMode의 Default Pawn Class를 기존에 생성한 Blueprint Character로 설정한다

<사진>

* 이제 Level에 생성된 Blueprint Character를 삭제하고 PlayStart로 대체한다

