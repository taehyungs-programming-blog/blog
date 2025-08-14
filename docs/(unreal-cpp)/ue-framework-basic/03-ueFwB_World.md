---
layout: default
title: "03. Game, Physics, Render World"
parent: "(Unreal Framwork의 이해)"
grand_parent: "(Unreal C++ 🚀)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## 큰 흐름 (프레임마다)

* Game World (게임 스레드)
    * UWorld가 레벨/액터/컴포넌트의 “진짜 상태(Truth)”를 틱으로 갱신.
    * 입력 처리, 게임 로직, 액터 스폰/파괴, 컴포넌트 속성 변경.
* Physics World (물리 스레드/서브스텟)
    * Game World가 넘겨준 변위/힘을 바탕으로 Chaos가 시뮬레이션.
    * 충돌/정지/슬리핑 등 계산 후 결과 변위를 백 버퍼에 저장.
* Render World (렌더 스레드)
    * Game World의 **SceneProxy(렌더링 전용 스냅샷)**를 이용해 드로우 커맨드 생성.
    * RHI를 통해 GPU에 제출.

* 스레드 간 데이터 전달
    * Game→Physics: 컴포넌트의 물리 플래그/힘/구속 조건 등 복사.
    * Physics→Game: 시뮬 결과(트랜스폼 등)를 안전 시점에 반영(일반적으로 다음 프레임).
    * Game→Render: CreateSceneProxy()로 렌더 전용 복제물을 만들어 렌더 스레드에 전달.

서로 직접 참조하지 않고 “복사/큐잉” 중심이라, 데이터 레이스 최소화가 핵심.

---

## 예를 들어보자.

### Game World (게임 스레드)

* **UWorld**를 중심으로 액터(Actor), 컴포넌트(Component), 게임 로직이 동작하는 영역.
* 입력 처리, AI, 스폰/파괴, 속성 변경 등 게임의 ‘진짜 상태’를 관리.
* Physics World, Render World로 데이터를 복사해서 전달하고, 직접 참조하지 않음.

```cpp
// 게임 스레드에서 액터 스폰
FActorSpawnParameters Params;
Params.Name = TEXT("MyCube");
AStaticMeshActor* Cube = GetWorld()->SpawnActor<AStaticMeshActor>(
    AStaticMeshActor::StaticClass(),
    FTransform(FRotator::ZeroRotator, FVector(0,0,100))
);
Cube->SetMobility(EComponentMobility::Movable);

```

* 여기서 GetWorld()->SpawnActor는 Game World의 UWorld에 새로운 액터를 등록.
* 이 시점에서는 Physics World나 Render World에는 전혀 반영 안 됨
* 프레임 끝에 엔진이 Physics/RHI 쪽에 “업데이트 요청”을 큐잉함.

### Physics World (물리 스레드 / Chaos)

* Game World에서 넘겨준 트랜스폼, 속도, 힘 등을 기반으로 Chaos 물리 시뮬레이션 실행.
* 독립 스레드에서 돌아가기 때문에, Game World와 직접 변수 공유 안 함.
* 결과(위치, 회전)는 다음 안전한 시점에 Game World로 복사.

```cpp
UStaticMeshComponent* Comp = FindComponentByClass<UStaticMeshComponent>();
Comp->SetSimulatePhysics(true);   // Chaos에 물리 바디 생성
Comp->AddImpulse(FVector(0, 0, 50000)); // 물리 힘 추가
```

* SetSimulatePhysics(true)를 호출하면, 엔진이 Physics World에 **물리 바디(BodyInstance)**를 생성해 등록.
* AddImpulse는 “다음 물리 스텝에서 이 힘을 적용하라”는 명령을 Physics World 큐에 추가.
* 실제 위치 변경은 Game World에 바로 반영되지 않고, Chaos가 계산 후 다음 틱에서 가져옴.

### Render World (렌더 스레드)

* Game World 상태를 기반으로 만든 **렌더링 전용 스냅샷(SceneProxy)**를 GPU에 전달.
* Render World는 Game World와 달리 읽기 전용 데이터만 사용.
* 업데이트는 게임 스레드 → 렌더 스레드 방향의 “명령 큐”로 전달됨.

```cpp
class URotatingTriangleComponent : public UPrimitiveComponent {
public:
  virtual FPrimitiveSceneProxy* CreateSceneProxy() override;
  virtual void TickComponent(float Dt, ELevelTick, FActorComponentTickFunction*) override {
    AddLocalRotation(FRotator(0, 45.f * Dt, 0));
    SendRenderTransform_Concurrent(); // 렌더 스레드에 트랜스폼 업데이트 요청
  }
};

class FTriangleSceneProxy : public FPrimitiveSceneProxy {
public:
  virtual void GetDynamicMeshElements(...) const override {
    // 단순 라인 3개로 삼각형 그리기
    PDI->DrawLine(A, B, FLinearColor::Red, SDPG_World, 2.0f);
    PDI->DrawLine(B, C, FLinearColor::Green, SDPG_World, 2.0f);
    PDI->DrawLine(C, A, FLinearColor::Blue, SDPG_World, 2.0f);
  }
};
```

* CreateSceneProxy()에서 렌더 스레드 전용 객체(FPrimitiveSceneProxy) 생성.
* TickComponent에서 SendRenderTransform_Concurrent()를 호출하면, 렌더 스레드에 안전하게 변환 행렬 전달.
* GetDynamicMeshElements는 Render World에서만 호출돼 GPU 드로우 커맨드 생성.