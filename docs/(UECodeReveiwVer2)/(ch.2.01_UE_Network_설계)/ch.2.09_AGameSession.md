---
layout: default
title: "02-09. AGameSession"
parent: "([Network] 01. UE Network 설계)"
grand_parent: "(UnrealEngine Code-Review Ver.2)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## 주요역할

* 로그인 관리

```cpp
// 로그인 승인 처리
virtual FString ApproveLogin(const FString& Options)
{
    // 로그인 검증 로직 수행
    // 에러 메시지 반환 (비어있으면 승인)
}
```

* 세션 관리
    * 게임 세션의 생성, 시작, 종료를 처리
    * 플레이어 수 제한 관리
    * 세션 접근 권한 관리 (비공개/공개 등)
* 네트워크 관련
    * 서버와 클라이언트 간의 연결 관리
    * 플레이어 킥/밴 기능
    * 세션 관련 네트워크 이벤트 처리
* 게임 규칙 적용

```cpp
// 예시: 최대 플레이어 수 제한
virtual bool AllowNewPlayer(const FString& Options)
{
    return GetNumberOfPlayers() < MaxPlayers;
}
```

---

## Example

```cpp
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameSession.h"
#include "CustomGameSession.generated.h"

UCLASS()
class GAME_API ACustomGameSession : public AGameSession
{
    GENERATED_BODY()

public:
    ACustomGameSession();

    // 로그인 승인 오버라이드
    virtual FString ApproveLogin(const FString& Options) override;
    
    // 새로운 플레이어 참가 허용 여부
    virtual bool AllowNewPlayer(const FString& Options) override;

protected:
    // 최대 플레이어 수
    UPROPERTY(Config)
    int32 MaxSessionPlayers;

    // VIP 플레이어 목록
    UPROPERTY()
    TArray<FString> VIPPlayers;

    // 밴 목록
    UPROPERTY()
    TArray<FString> BannedPlayers;
};
```