---
layout: default
title: "24. PrimitiveComponent"
parent: "(Unreal Framwork의 이해)"
grand_parent: "(Unreal C++ 🚀)"
nav_order: 3
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## 개요

* PrimitiveComponent는 Unreal Engine에서 모든 물리적 상호작용을 추상화한 핵심 컴포넌트
* 이 컴포넌트는 렌더링, 충돌 감지, 물리 시뮬레이션 등의 기능을 제공하며, 게임 오브젝트의 시각적 표현과 물리적 행동을 정의

## 컴포넌트 계층 구조

```
PrimitiveComponent
├── MeshComponent (렌더링)
│   ├── StaticMeshComponent (정적 메시)
│   ├── SkeletalMeshComponent (스켈레톤 메시)
│   └── InstancedStaticMeshComponent (인스턴스 메시)
└── ShapeComponent (충돌)
    ├── BoxComponent (박스 충돌)
    ├── CapsuleComponent (캡슐 충돌)
    ├── SphereComponent (구 충돌)
    └── StaticMeshComponent (복잡한 충돌)
```

## 주요 기능

### 1. 물리 시뮬레이션
- **Rigid Body Dynamics**: 강체 물리 시뮬레이션
- **Collision Detection**: 충돌 감지 및 응답
- **Physics Materials**: 물리 재질 설정

### 2. 렌더링
- **Mesh Rendering**: 3D 메시 렌더링
- **Material System**: 머티리얼 시스템 통합
- **LOD (Level of Detail)**: 거리에 따른 디테일 조절

### 3. 트랜스폼
- **Position, Rotation, Scale**: 위치, 회전, 크기 조절
- **World/Local Space**: 월드/로컬 좌표계 변환

## 예시 코드

### 1. 기본 PrimitiveComponent 생성

```cpp
// 헤더 파일 (.h)
#pragma once

#include "CoreMinimal.h"
#include "Components/PrimitiveComponent.h"
#include "GameFramework/Actor.h"
#include "MyPrimitiveActor.generated.h"

UCLASS()
class MYGAME_API AMyPrimitiveActor : public AActor
{
    GENERATED_BODY()

public:
    AMyPrimitiveActor();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

    // PrimitiveComponent 참조
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class UPrimitiveComponent* PrimitiveComp;

    // 물리 설정
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics")
    bool bEnablePhysics;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics")
    float Mass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics")
    float LinearDamping;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics")
    float AngularDamping;
};
```

```cpp
// 구현 파일 (.cpp)
#include "MyPrimitiveActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Engine/Engine.h"

AMyPrimitiveActor::AMyPrimitiveActor()
{
    PrimaryActorTick.bCanEverTick = true;

    // StaticMeshComponent 생성 (PrimitiveComponent의 자식)
    PrimitiveComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PrimitiveComponent"));
    RootComponent = PrimitiveComp;

    // 기본 물리 설정
    bEnablePhysics = true;
    Mass = 1.0f;
    LinearDamping = 0.1f;
    AngularDamping = 0.1f;
}

void AMyPrimitiveActor::BeginPlay()
{
    Super::BeginPlay();
    
    // 물리 설정 적용
    if (PrimitiveComp)
    {
        PrimitiveComp->SetSimulatePhysics(bEnablePhysics);
        PrimitiveComp->SetMassOverrideInKg(NAME_None, Mass);
        PrimitiveComp->SetLinearDamping(LinearDamping);
        PrimitiveComp->SetAngularDamping(AngularDamping);
    }
}

void AMyPrimitiveActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    // 물리 상태 확인
    if (PrimitiveComp && PrimitiveComp->IsSimulatingPhysics())
    {
        FVector CurrentVelocity = PrimitiveComp->GetPhysicsLinearVelocity();
        FVector CurrentAngularVelocity = PrimitiveComp->GetPhysicsAngularVelocity();
        
        // 속도 정보 출력 (디버그용)
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Green, 
                FString::Printf(TEXT("Linear Velocity: %s"), *CurrentVelocity.ToString()));
        }
    }
}
```

### 2. 충돌 이벤트 처리

```cpp
// 헤더 파일에 추가
UCLASS()
class MYGAME_API AMyPrimitiveActor : public AActor
{
    // ... 기존 코드 ...

    // 충돌 이벤트 함수들
    UFUNCTION()
    void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, 
               UPrimitiveComponent* OtherComp, FVector NormalImpulse, 
               const FHitResult& Hit);

    UFUNCTION()
    void OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, 
                       UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
                       bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    void OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, 
                     UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
```

```cpp
// 구현 파일에 추가
void AMyPrimitiveActor::BeginPlay()
{
    Super::BeginPlay();
    
    if (PrimitiveComp)
    {
        // 충돌 이벤트 바인딩
        PrimitiveComp->OnComponentHit.AddDynamic(this, &AMyPrimitiveActor::OnHit);
        PrimitiveComp->OnComponentBeginOverlap.AddDynamic(this, &AMyPrimitiveActor::OnBeginOverlap);
        PrimitiveComp->OnComponentEndOverlap.AddDynamic(this, &AMyPrimitiveActor::OnEndOverlap);
        
        // 충돌 설정
        PrimitiveComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        PrimitiveComp->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
        PrimitiveComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
    }
}

void AMyPrimitiveActor::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, 
                             UPrimitiveComponent* OtherComp, FVector NormalImpulse, 
                             const FHitResult& Hit)
{
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, 
            FString::Printf(TEXT("Hit: %s"), *OtherActor->GetName()));
    }
    
    // 충돌 시 힘 적용
    if (OtherComp && OtherComp->IsSimulatingPhysics())
    {
        FVector Impulse = NormalImpulse * 1000.0f;
        OtherComp->AddImpulse(Impulse, NAME_None, true);
    }
}

void AMyPrimitiveActor::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, 
                                      UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
                                      bool bFromSweep, const FHitResult& SweepResult)
{
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, 
            FString::Printf(TEXT("Begin Overlap: %s"), *OtherActor->GetName()));
    }
}

void AMyPrimitiveActor::OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, 
                                    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, 
            FString::Printf(TEXT("End Overlap: %s"), *OtherActor->GetName()));
    }
}
```

### 3. 물리 힘 적용

```cpp
// 헤더 파일에 추가
UCLASS()
class MYGAME_API AMyPrimitiveActor : public AActor
{
    // ... 기존 코드 ...

    // 물리 힘 적용 함수들
    UFUNCTION(BlueprintCallable, Category = "Physics")
    void ApplyForce(FVector Force, FName BoneName = NAME_None);

    UFUNCTION(BlueprintCallable, Category = "Physics")
    void ApplyImpulse(FVector Impulse, FName BoneName = NAME_None);

    UFUNCTION(BlueprintCallable, Category = "Physics")
    void ApplyTorque(FVector Torque);

    UFUNCTION(BlueprintCallable, Category = "Physics")
    void SetPhysicsLinearVelocity(FVector NewVelocity);

    UFUNCTION(BlueprintCallable, Category = "Physics")
    void SetPhysicsAngularVelocity(FVector NewAngularVelocity);
};
```

```cpp
// 구현 파일에 추가
void AMyPrimitiveActor::ApplyForce(FVector Force, FName BoneName)
{
    if (PrimitiveComp && PrimitiveComp->IsSimulatingPhysics())
    {
        PrimitiveComp->AddForce(Force, BoneName);
    }
}

void AMyPrimitiveActor::ApplyImpulse(FVector Impulse, FName BoneName)
{
    if (PrimitiveComp && PrimitiveComp->IsSimulatingPhysics())
    {
        PrimitiveComp->AddImpulse(Impulse, BoneName, true);
    }
}

void AMyPrimitiveActor::ApplyTorque(FVector Torque)
{
    if (PrimitiveComp && PrimitiveComp->IsSimulatingPhysics())
    {
        PrimitiveComp->AddTorqueInDegrees(Torque);
    }
}

void AMyPrimitiveActor::SetPhysicsLinearVelocity(FVector NewVelocity)
{
    if (PrimitiveComp && PrimitiveComp->IsSimulatingPhysics())
    {
        PrimitiveComp->SetPhysicsLinearVelocity(NewVelocity);
    }
}

void AMyPrimitiveActor::SetPhysicsAngularVelocity(FVector NewAngularVelocity)
{
    if (PrimitiveComp && PrimitiveComp->IsSimulatingPhysics())
    {
        PrimitiveComp->SetPhysicsAngularVelocityInDegrees(NewAngularVelocity);
    }
}
```

### 4. Blueprint에서 사용할 수 있는 함수들

```cpp
// 헤더 파일에 추가
UCLASS()
class MYGAME_API AMyPrimitiveActor : public AActor
{
    // ... 기존 코드 ...

    // Blueprint에서 사용할 수 있는 함수들
    UFUNCTION(BlueprintCallable, Category = "Primitive Component")
    FVector GetComponentLocation() const;

    UFUNCTION(BlueprintCallable, Category = "Primitive Component")
    FRotator GetComponentRotation() const;

    UFUNCTION(BlueprintCallable, Category = "Primitive Component")
    FVector GetComponentScale() const;

    UFUNCTION(BlueprintCallable, Category = "Primitive Component")
    void SetComponentLocation(FVector NewLocation);

    UFUNCTION(BlueprintCallable, Category = "Primitive Component")
    void SetComponentRotation(FRotator NewRotation);

    UFUNCTION(BlueprintCallable, Category = "Primitive Component")
    void SetComponentScale(FVector NewScale);

    UFUNCTION(BlueprintCallable, Category = "Primitive Component")
    bool IsSimulatingPhysics() const;

    UFUNCTION(BlueprintCallable, Category = "Primitive Component")
    void SetSimulatePhysics(bool bSimulate);
};
```

```cpp
// 구현 파일에 추가
FVector AMyPrimitiveActor::GetComponentLocation() const
{
    return PrimitiveComp ? PrimitiveComp->GetComponentLocation() : FVector::ZeroVector;
}

FRotator AMyPrimitiveActor::GetComponentRotation() const
{
    return PrimitiveComp ? PrimitiveComp->GetComponentRotation() : FRotator::ZeroRotator;
}

FVector AMyPrimitiveActor::GetComponentScale() const
{
    return PrimitiveComp ? PrimitiveComp->GetComponentScale() : FVector::OneVector;
}

void AMyPrimitiveActor::SetComponentLocation(FVector NewLocation)
{
    if (PrimitiveComp)
    {
        PrimitiveComp->SetWorldLocation(NewLocation);
    }
}

void AMyPrimitiveActor::SetComponentRotation(FRotator NewRotation)
{
    if (PrimitiveComp)
    {
        PrimitiveComp->SetWorldRotation(NewRotation);
    }
}

void AMyPrimitiveActor::SetComponentScale(FVector NewScale)
{
    if (PrimitiveComp)
    {
        PrimitiveComp->SetWorldScale3D(NewScale);
    }
}

bool AMyPrimitiveActor::IsSimulatingPhysics() const
{
    return PrimitiveComp ? PrimitiveComp->IsSimulatingPhysics() : false;
}

void AMyPrimitiveActor::SetSimulatePhysics(bool bSimulate)
{
    if (PrimitiveComp)
    {
        PrimitiveComp->SetSimulatePhysics(bSimulate);
    }
}
```

## 주요 속성 및 설정

### 1. 물리 속성
- **Mass**: 질량
- **Linear Damping**: 선형 감쇠
- **Angular Damping**: 각속도 감쇠
- **Gravity Scale**: 중력 스케일
- **Physics Material**: 물리 재질

### 2. 충돌 설정
- **Collision Enabled**: 충돌 활성화 여부
- **Collision Object Type**: 충돌 객체 타입
- **Collision Response**: 충돌 응답 설정

### 3. 렌더링 설정
- **Cast Shadow**: 그림자 캐스팅
- **Visible**: 가시성
- **Hidden in Game**: 게임에서 숨김
- **Render Custom Depth**: 커스텀 깊이 렌더링

## 사용 시나리오

### 1. 게임 오브젝트
- 플레이어, 적, 아이템 등의 기본 컴포넌트
- 물리 시뮬레이션이 필요한 모든 오브젝트

### 2. 환경 오브젝트
- 바위, 나무, 건물 등의 정적 오브젝트
- 상호작용 가능한 환경 요소

### 3. 특수 효과
- 파티클 시스템과 연동
- 물리 기반 특수 효과

## 주의사항

1. **성능 최적화**: 불필요한 물리 시뮬레이션 비활성화
2. **메모리 관리**: 컴포넌트 참조의 적절한 관리
3. **충돌 설정**: 게임플레이에 맞는 충돌 설정
4. **물리 재질**: 적절한 물리 재질 사용


