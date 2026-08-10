---
layout: default
title: "02. Actor"
parent: "(Example2 - 3D 바람의 나라)"
grand_parent: "(Unreal C++ 🚀)"
great_grand_parent: "Legacy Archive"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌎](https://github.com/Arthur880708/Unreal_Cpp_Basic_2/tree/2)

## Actor

* 월드에 속한 컨텐츠의 기본 단위
* Actor는 Transform을 가지며, 월드로 부터 Tick, Timer Service를 제공 받는다
* Actor의 구성은 Component로 구성이 되어 있으며 기본적으로 Root Component하에 다른 Component들이 붙는다.

---

### 실습해보기 - BlueSprint로 Actor만들기

* 기존 Map을 복사하여 Step3로 만들자
* ArenaBattle\Blueprint\BP_Fountain을 새로 만들어보고자 한다.
    * 기존에 있다면 삭제할 것
* Add -> Blueprint Class -> Actor로 생성
* 생성된 BP_Fountain에 Compnents -> Add -> Static Mesh Component를 추가하자.
* Static Mesh를 Root Compnent로 지정하자.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_2/ucpp_2_2_1.png"/>
</p>

* Static Mesh를 SM_Plains_Castle_Fountain으로 변경하자.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_2/ucpp_2_2_2.png"/>
</p>

* Body아래 Water라는 Static Mesh를 추가하고 Static Mesh에 적당한 Mesh를 넣어주자.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_2/ucpp_2_2_3.png"/>
</p>

### 실습해보기 - C++로 Actor만들기

* Tools -> New C++ Class -> Actor로 생성
* `D:/Git/Unreal_Cpp_Basic_2/Source/ArenaBattle/Prop/`
* ABFountain이란 이름으로 생성

* 주의할 점은 C++ Header를 수정할 경우 **UE5 Editor**를 한번 **종료**해 주는것이 좋다

```cpp
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ABFountain.generated.h"

UCLASS()
class ARENABATTLE_API AABFountain : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AABFountain();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category=Mesh)
	TObjectPtr<class UStaticMeshComponent> Body;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Mesh)
	TObjectPtr<class UStaticMeshComponent> Water;

};
```

```cpp
AABFountain::AABFountain()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	Body = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BODY"));
	Water = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WATER"));

	RootComponent = Body;
	Water->SetupAttachment(Body);
	Water->SetRelativeLocation(FVector(0.0f, 0.0f, 132.0f));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> BodyMeshRef(TEXT("/Game/ArenaBattle/Environment/Props/SM_Plains_Castle_Fountain_01.SM_Plains_Castle_Fountain_01"));
	if (BodyMeshRef.Object)
	{
		Body->SetStaticMesh(BodyMeshRef.Object);
	}

	static ConstructorHelpers::FObjectFinder<UStaticMesh> WaterMeshRef(TEXT("/Game/ArenaBattle/Environment/Props/SM_Plains_Fountain_02.SM_Plains_Fountain_02"));
	if (WaterMeshRef.Object)
	{
		Water->SetStaticMesh(WaterMeshRef.Object);
	}
}
```

* 빌드(Ctrl+Alt+F11) !!!

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_2/ucpp_2_2_4.png"/>
</p>

---

### 이런식의 활용도 가능하다

* BP_Fountain의 Parent Class를 ABFountain으로 변경하자.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_2/ucpp_2_2_5.png"/>
</p>

* C++로 구현된 Actor를 Blueprint를 활용해 확장이 가능하다

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_2/ucpp_2_2_6.png"/>
</p>

---

