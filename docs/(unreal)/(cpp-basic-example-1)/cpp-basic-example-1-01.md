---
layout: default
title: "01. 이 정도는 알아야 진행이 됨 ..."
parent: "(Unreal C++ - Basic Example)"
grand_parent: "(Unreal 🚀)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## 환경, 프로젝트 세팅

* Unreal 5.0.X
* Visual Studio 2019
* 기본맵 세팅
    * 기본 시작된 Map을 MainMap으로 저장 후
    * Project Settings -> Maps & Modes로 들어가 기본 Map으로 설정한다
* Visual Studio 세팅
    * 2022를 사용하는 것을 추천(2019도 상관은 없음)
    * C++를 사용한 게임 개발을 설치하며 **Unreal Engine 설치 관리자**가 포함되어야 함을 주의

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/cpp-basic-example-1/cpp-basic-example-1-1.png"/>
</p>

* Visual Studio Code Extension
    * C/C++
    * Unreal Engine 4 Snippets

---

## 캐릭터 커스텀, 캐릭터 화면 전환이 어색하다면?

* Stylized Character Kit: Casual01에서 ThirdPersonCharacter Mesh를 하나가져온다
* 가져온 ThirdPersonCharacter Mesh를 BP_ThirdPersonCharacter라는 이름의 Blueprint Class로 저장한다
* Blueprint Class 내부
    * Auto Possess Player를 Player0로 설정한다
* 상속한 ThirdPersonChacter Event Graph에 Warning이 두 개 나있을건데 아래와 같이 해결한다

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/cpp-basic-example-1/cpp-basic-example-1-2.png"/>
</p>

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/cpp-basic-example-1/cpp-basic-example-1-3.png"/>
</p>

---

## C++ 컴파일

* Editor 수정
    * Edit -> Editor Perferences -> General -> Source Code -> Visual Studio Code
* Tools -> New C++ Class를 통해 Actor하나를 생성해보자
* 생성 후 Tools -> Open Visual Studio Code 실행
    * Unreal Eidtor 없이 Visual Studio Code만 열고 싶다면 .code-workspace파일을 우클릭 후 Visual Studio Code로 열기를 하면 된다.
* 컴파일
    * Terminal -> Run Build Task...(Ctrl+Shift+B) 클릭
    * 다양한 빌드옵션이 있는데 Win64 Development Build를 선택해야한다

---

## UPROPERTY

* 아래와 같이 수정 후 Editor에서 Recompile And Reload버튼을 클릭

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/cpp-basic-example-1/cpp-basic-example-1-4.png"/>
</p>

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

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/cpp-basic-example-1/cpp-basic-example-1-5.png"/>
</p>

---

## 충돌처리

* 현재의 문제점은 충돌이 되면 카메라 처리가 이상하게 된다. + 캐릭터가 밀리지 않는다

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/cpp-basic-example-1/cpp-basic-example-1-6.png"/>
</p>

* 해결법은 일종의 편법을 사용해야한다
* MoveUpdate를 호출하여 캐릭터의 움직임의 변화를 매 Tick체크해야함

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/cpp-basic-example-1/cpp-basic-example-1-7.png"/>
</p>

---

## 캐릭터 시작점 설정하기

* 우선 아래와 같이 GameMode를 생성해보자

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/cpp-basic-example-1/cpp-basic-example-1-8.png"/>
</p>

* 생성된 GameMode의 Default Pawn Class를 기존에 생성한 Blueprint Character로 설정한다

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/cpp-basic-example-1/cpp-basic-example-1-9.png"/>
</p>

* 이제 Level에 생성된 Blueprint Character를 삭제하고 PlayStart로 대체한다

---

## Line and Shape Trace

* 물건을 어떻게 잡을수 있는가?
* 역시 Collision을 이용, 간단히 Line이나 Shape를 쏴 앞에 있는 물체의 Collsion을 얻어오는 방식이다
* Project Settings -> Engine -> Collision -> Trace Channels 에서 New Trace Channel로 새채널을 만든다

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/cpp-basic-example-1/cpp-basic-example-1-10.png"/>
</p>

* 잡을수 있는 Object의 Collision Preset을 Custom으로 둔다

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/cpp-basic-example-1/cpp-basic-example-1-11.png"/>
</p>

