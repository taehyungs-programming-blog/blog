---
layout: default
title: "01. UE Class"
parent: "(Unreal Framwork의 이해)"
grand_parent: "(Unreal C++ 🚀)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

> UE에서 이해하면 좋을 대표 클래스의 간략한 설명.

---

## UE 대표 클래스

### 접두어 정리

> 사전지식임

* AClass: Actor 파생. 월드에 스폰/배치 가능(트랜스폼 존재).
* UClass: UObject 파생. 리소스/관리/컴포넌트/데이터 등 월드 공간에 직접 존재하지 않음(트랜스폼 없음).
* (참고로 자주 쓰는 다른 접두사)
    * F: 값 타입(Struct)
    * I: 인터페이스
    * E: 열거형(Enum)
    * T: 템플릿(컨테이너 등)

### UEngine

* 엔진의 최상위 매니저. 렌더링/오디오/넷/플랫폼 서브시스템 초기화, UGameInstance 생성, UGameViewportClient 연결 등을 총괄.

### UGameViewportClient

* 게임 “창(viewport)”의 관리자. 플랫폼(OS)별 입력 처리와 화면 출력 파이프라인을 중계.
* 엔진 레벨(UEngine)에서 만든 플랫폼 창/입력을 게임 인스턴스(UGameInstance)와 로컬 플레이어들에게 전달하기 위한 접점

### UGameInstance

* “앱 실행 동안” 유지되는 게임 컨테이너(맵 전환 간에도 유지). 전역 상태/서브시스템(온라인, 세이브, 매치메이킹 등) 보관.

### UWorld

* 하나의 월드(레벨 집합). AGameMode/AGameState(게임 규칙/상태), NetDriver, 타이머, 티킹 루프를 품고 모든 액터를 관리.

### ULevel

* UWorld 안에 로드되는 씬 단위(퍼시스턴트/스트리밍 레벨). 액터 컬렉션을 보유.
    * “AActor는 Level에 들어가는 최소 단위”

### AActor

* 월드에 “배치/스폰” 가능한 모든 것의 기본 클래스(트랜스폼 有). 컴포넌트(씬/렌더/콜리전 등)를 붙여 기능 확장.
    * 레벨에 들어가는 최소 단위라는 점을 기억!

### APawn

* **컨트롤러(플레이어나 AI)**가 조종 가능한 액터. 이동/회전/카메라 장착 등 플레이어러블의 본체.

### AGameMode · AGameState

* AGameMode: 규칙/승패/스폰/사용 클래스(HUD, PC, Pawn 등) 결정. 서버에만 존재.
* AGameState: 매치 진행 상황, 팀/스코어/남은 시간 등 클라이언트로 복제(replication) 되는 공유 상태.
    * 서버의 GameMode가 값을 GameState에 적으면 그 값이 클라이언트에 복제되어 모두가 읽음

### APlayerController · APlayerState

* APlayerController (PC): 입력을 받아 **월드에서의 ‘나’**를 대표. 카메라/포제션(폰 소유)/UI 상호작용/서버 RPC 등 수행. (서버와 소유 클라에 존재)
* APlayerState (PS): 플레이어 점수/팀/닉 등 플레이어별 메타데이터를 저장, 모든 클라로 복제.
    * PlayerState를 통해 다른 클라이언트가 서로의 정보를 공유

### ULocalPlayer

* 로컬 장치의 ‘진짜 나’(입력 장치/플레이어 인덱스/스플릿스크린 슬롯) 표현. Viewport의 화면/오디오 뷰와 연결되어 로컬 입력 소스를 가집니다.
* 비유: APlayerController는 ‘월드 상의 나’, ULocalPlayer는 ‘물리적 PC/콘솔의 나(로컬 사용자)’.

### APlayerCameraManager

* 해당 PlayerController의 카메라 상태(뷰포인트, FOV, 포스트프로세스, 흔들림 등)를 관리.

### UPlayerInput

* 키/패드/마우스 축·액션 맵핑을 담는 입력 매핑 컨테이너. (프로젝트 세팅 Input과 연결)

### UInputComponent

* 액터/폰/컨트롤러에 붙여 입력 바인딩(“Jump”, “Fire”, 축 등)을 실제 함수로 연결. 소유한 액터가 활성일 때 입력 이벤트를 수신.

### UCameraComponent

* 액터/컴포넌트 트리에 붙여 카메라 뷰를 제공. PlayerCameraManager가 이 뷰를 최종 조합/보정.

---

## 관계 다이어그램

```
UEngine
  └─ UGameInstance
       ├─ UWorld
       │    ├─ AGameMode (Server Only)
       │    │     └─ AGameState (Replicated to Clients)
       │    ├─ APlayerController (owns) ── APlayerState (Replicated)
       │    │         └─ (possesses) APawn ── UInputComponent / UCameraComponent
       │    └─ AActor ...
       └─ UGameViewportClient
            └─ ULocalPlayer(들) ── APlayerController 연결
```