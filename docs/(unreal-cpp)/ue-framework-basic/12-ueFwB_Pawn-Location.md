---
layout: default
title: "12. Pawn의 생성위치를 지정해 보자"
parent: "(Unreal Framwork의 이해)"
grand_parent: "(Unreal C++ 🚀)"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## 왜 필요할까?

* 이게 왜 중요하지? 그냥 Player에 Pawn의 생성위치를 지정해 버리면안되나?
    * 그럼 만약 Map이 없는데 Player를 둔다면? 충돌이 있는곳에 둔다면?
    * 이러제약을 Engine에서 어떻게 처리하는지 확인해 보자

---

## 이론

* 스폰 위치 결정 우선순위

### 이름으로 지정된 PlayerStart
    
* 레벨 로딩 옵션(또는 트래블 URL)에 StartSpot=PlayerStart이름 이 들어오면, `GameModeBase::FindPlayerStart(Controller, IncomingName)`가 그 **이름과 일치하는 APlayerStart**를 우선 반환
    * 예) `UGameplayStatics::OpenLevel(GetWorld(), "MyMap", true, "StartSpot=PS_Red");`

### PlayerController에 이미 정해진 StartSpot

* 컨트롤러의 `AActor* StartSpot`이 유효하면 그 위치를 사용
* 기본 구현에서 `GameModeBase::ShouldSpawnAtStartSpot(PlayerController)`가 true이면 RestartPlayer 흐름이 StartSpot을 그대로 쓴다
    * (예: 서버 트래블/리스폰 과정에서 컨트롤러에 StartSpot이 유지된 경우 등)

### GameMode의 ChoosePlayerStart(순회/선택 로직)

* 이름 지정/StartSpot 둘 다 없으면 **AGameModeBase::ChoosePlayerStart_Implementation**가 월드의 APlayerStart들을 순회하며 하나를 고른다
* 팀 태그, 거리, 사용 중 여부 등 **게임 규칙에 맞는 “베스트 Start”**를 찾아 반환하는 지점
* 커스텀 규칙(팀 배치, 특정 태그/필터 등)은 보통 이 함수 오버라이드로 구현

### 모두 실패 시: WorldSettings 기반의 기본 위치

* 적절한 PlayerStart를 못 찾으면 **월드 기본 트랜스폼(보통 월드 원점/WorldSettings 기준)**으로 스폰을 시도
* 맵에 PlayerStart가 하나도 없으면 종종 원점(0,0,0)에서 스폰되거나, 프로젝트/월드 설정의 기본 동작(예: 캡슐 충돌로 실패)으로 이어질 수 있으니 최소 1개의 PlayerStart를 두는 것이 안전

---

## “충돌 있음/없음”에 따른 분기(스폰 충돌 처리)

* 스폰 시 충돌 처리는 Pawn의 SpawnCollisionHandlingMethod(또는 Spawn 파라미터)로 제어

* `AlwaysSpawn` : 충돌 여부와 관계없이 스폰(위치 겹침 가능).
* `AdjustIfPossibleButAlwaysSpawn` (권장 기본값) : 먼저 충돌 확인 → 가능하면 주변으로 살짝 보정해서 스폰 → 그래도 안 되면 강제로 스폰.
* `AdjustIfPossibleButDontSpawnIfColliding` : 보정 시도 후 여전히 충돌이면 스폰 포기.
* `DontSpawnIfColliding` : 충돌이면 즉시 스폰 포기.

---

## Example

```cpp
// MyGameMode.h
#pragma once
#include "GameFramework/GameModeBase.h"
#include "MyGameMode.generated.h"

UCLASS()
class AMyGameMode : public AGameModeBase
{
    GENERATED_BODY()
public:
    virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
    virtual bool ShouldSpawnAtStartSpot(AController* Player) override { return true; } // StartSpot 우선 사용
};

```

```cpp
// MyGameMode.cpp
#include "MyGameMode.h"
#include "GameFramework/PlayerStart.h"
#include "EngineUtils.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"

// 충돌 검사 유틸: 주어진 Transform에서 이 PawnClass가 겹치는지 대략 판단
static bool IsSpawnFree(UWorld* World, UClass* PawnClass, const FTransform& T)
{
    if (!World || !PawnClass) return false;

    // 캐릭터 캡슐 기준(프로젝트에 맞게 조정)
    const float Radius = 34.f;
    const float HalfHeight = 88.f;

    FCollisionShape Capsule = FCollisionShape::MakeCapsule(Radius, HalfHeight);
    FCollisionQueryParams Params(SCENE_QUERY_STAT(SpawnOverlap), false);
    FCollisionResponseParams Resp;

    // 지면 살짝 위로(바닥 파고듦 방지)
    FVector Pos = T.GetLocation() + FVector(0,0,HalfHeight + 2.f);
    FQuat Rot = T.GetRotation();

    // 월드에 겹치는 바디가 있는지
    bool bBlocking = World->OverlapBlockingTestByChannel(
        Pos, Rot, ECC_Pawn, Capsule, Params, Resp
    );

    return !bBlocking;
}

AActor* AMyGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
    UWorld* World = GetWorld();
    if (!World) return Super::ChoosePlayerStart_Implementation(Player);

    // 1) URL 옵션 StartSpot=Name 이 있으면 엔진 기본 Find로 처리되지만,
    //    여기선 추가 규칙을 넣고 싶을 때를 가정하여 참고만 합니다.
    //    (기본적으로 AGameModeBase::FindPlayerStart가 먼저 이를 처리)

    // 2) 커스텀 선택: 팀 태그 "TeamA" + 충돌 없는 Start 우선
    TArray<APlayerStart*> Candidates_TagFirst;
    TArray<APlayerStart*> Candidates_Fallback;

    for (TActorIterator<APlayerStart> It(World); It; ++It)
    {
        APlayerStart* PS = *It;
        if (!IsValid(PS)) continue;

        const FTransform SpawnT(PS->GetActorRotation(), PS->GetActorLocation());
        UClass* PawnClass = GetDefaultPawnClassForController(Player);
        const bool bFree = IsSpawnFree(World, PawnClass, SpawnT);

        // 충돌 없는 곳만 우선 큐에 담고, 충돌 있으면 후순위
        bool bTagged = PS->ActorHasTag(FName("TeamA"));
        if (bFree)
        {
            (bTagged ? Candidates_TagFirst : Candidates_Fallback).Add(PS);
        }
    }

    // 태그 있는 깨끗한 위치 → 태그 없는 깨끗한 위치 → 엔진 기본
    if (Candidates_TagFirst.Num() > 0) return Candidates_TagFirst[0];
    if (Candidates_Fallback.Num() > 0)  return Candidates_Fallback[0];

    // 전부 막혀 있으면 기본 구현(보정 스폰 or 다른 규칙)에 맡김
    return Super::ChoosePlayerStart_Implementation(Player);
}

```

```cpp
// 예: 특정 상황에서 컨트롤러 StartSpot 지정
void SetStartSpotAndRestart(AController* PC, AActor* Start)
{
    if (!PC || !Start) return;

    PC->StartSpot = Start; // ShouldSpawnAtStartSpot==true면 이 위치를 그대로 사용
    if (AGameModeBase* GM = PC->GetWorld()->GetAuthGameMode<AGameModeBase>())
    {
        GM->RestartPlayer(PC); // StartSpot에서 Pawn 스폰
    }
}

```

```cpp
// Pawn 클래스(또는 BP)에서 기본 스폰 충돌 정책을 설정
APawn::APawn()
{
    SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
    // 또는 AlwaysSpawn / AdjustIfPossibleButAlwaysSpawn / DontSpawnIfColliding
}

```

> 좀 더 상세한 엔진적 구현은 다음장에서