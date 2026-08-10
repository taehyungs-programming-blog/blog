---
layout: default
title: "23. ActorComponent"
parent: "(Unreal Framwork의 이해)"
grand_parent: "(Unreal C++ 🚀)"
great_grand_parent: "Legacy Archive"
nav_order: 3
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## Actor의 기본 개념

### Actor란?
- **World에 배치할 수 있는 객체**의 기본 클래스
- Actor는 **World에 배치할 수 있다는 특성만** 가지고 있음
- 다른 특성들은 모두 **Component를 통해** 제공됨

### Actor의 구조
```
Actor
├── RootComponent (SceneComponent)
└── 기타 Components (선택적)
```

- **RootComponent**: World의 좌표를 담당하는 SceneComponent
- **기타 Components**: Actor의 실제 기능을 담당하는 다양한 Component들

## Component 시스템의 장점

### 1. 다중상속 문제 해결

#### 문제가 되는 다중상속 예시
```cpp
// 문제가 되는 다중상속 구조
class FlyingCar : public Car, public Airplane {
    // 다이아몬드 문제 발생 가능
    // Car와 Airplane이 모두 Vehicle을 상속받는다면?
};

class Vehicle {
    virtual void move() = 0;
};

class Car : public Vehicle {
    void move() override { /* 땅에서 이동 */ }
};

class Airplane : public Vehicle {
    void move() override { /* 하늘에서 이동 */ }
};

// FlyingCar::move()는 어떤 구현을 사용해야 할까?
```

#### Component 시스템으로 해결
```cpp
// Component 기반 구조
class FlyingCar : public Actor {
    UPROPERTY()
    class UCarMovementComponent* CarMovement;
    
    UPROPERTY()
    class UAirplaneMovementComponent* AirplaneMovement;
    
    UPROPERTY()
    class UEngineComponent* Engine;
    
    UPROPERTY()
    class UPhysicsComponent* Physics;
};

// 각 Component는 독립적으로 기능을 제공
// 충돌 없이 여러 기능을 조합 가능
```

### 2. 조합 기반 설계 (Composition over Inheritance)

#### 전통적인 상속 방식의 문제점
```cpp
// 상속 체인이 복잡해짐
class GameObject {
    virtual void update() {}
};

class MovableObject : public GameObject {
    virtual void update() override { /* 이동 로직 */ }
};

class RenderableObject : public MovableObject {
    virtual void update() override { /* 렌더링 + 이동 로직 */ }
};

class CollidableRenderableMovableObject : public RenderableObject {
    virtual void update() override { /* 충돌 + 렌더링 + 이동 로직 */ }
    // 새로운 기능 추가 시마다 새로운 클래스 필요
};
```

#### Component 방식의 유연성
```cpp
// Component 조합으로 유연한 설계
class Player : public Actor {
    UPROPERTY()
    class UMovementComponent* Movement;
    
    UPROPERTY()
    class URenderComponent* Render;
    
    UPROPERTY()
    class UCollisionComponent* Collision;
    
    UPROPERTY()
    class UHealthComponent* Health;
    
    UPROPERTY()
    class UInventoryComponent* Inventory;
};

class Enemy : public Actor {
    UPROPERTY()
    class UMovementComponent* Movement;
    
    UPROPERTY()
    class URenderComponent* Render;
    
    UPROPERTY()
    class UCollisionComponent* Collision;
    
    UPROPERTY()
    class UHealthComponent* Health;
    
    UPROPERTY()
    class UAIComponent* AI;  // Player와 다른 Component
};
```

## Component의 종류

### 1. SceneComponent
- **위치, 회전, 스케일** 정보를 가짐
- World 좌표계에서의 Transform 관리
- RootComponent로 사용 가능

### 2. PrimitiveComponent
- SceneComponent를 상속
- **물리적 표현**을 가짐 (Collision, Physics)
- Mesh, StaticMesh, SkeletalMesh 등

### 3. ActorComponent
- **기능적 로직**을 담당
- Transform 정보 없음
- Health, Inventory, AI 등의 로직

## 실제 사용 예시

### 플레이어 캐릭터 구성
```cpp
class APlayerCharacter : public APawn {
    // RootComponent (위치 담당)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    class UCapsuleComponent* CapsuleComponent;
    
    // 시각적 표현
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    class USkeletalMeshComponent* MeshComponent;
    
    // 카메라
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    class UCameraComponent* CameraComponent;
    
    // 이동
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    class UCharacterMovementComponent* MovementComponent;
    
    // 입력
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    class UInputComponent* InputComponent;
    
    // 생명력
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    class UHealthComponent* HealthComponent;
};
```

## Component 시스템의 장점 요약

1. **다중상속 문제 해결**: 다이아몬드 문제, 모호성 제거
2. **유연한 조합**: 필요한 기능만 선택적으로 조합
3. **코드 재사용성**: Component를 다른 Actor에서도 재사용 가능
4. **유지보수성**: 각 기능이 독립적으로 관리됨
5. **확장성**: 새로운 기능 추가 시 새로운 Component만 만들면 됨
6. **성능**: 필요한 Component만 활성화하여 최적화 가능

---

### 1. SceneComponent (상세 설명)

#### 기본 개념
- **Transform 정보를 가진 Component**의 기본 클래스
- **위치(Location), 회전(Rotation), 스케일(Scale)** 정보를 관리
- World 좌표계에서의 공간적 위치를 담당

#### 주요 특징
```cpp
class USceneComponent : public UActorComponent {
    // Transform 정보
    FVector RelativeLocation;    // 부모 기준 상대 위치
    FRotator RelativeRotation;   // 부모 기준 상대 회전
    FVector RelativeScale3D;     // 부모 기준 상대 스케일
    
    // 부모-자식 관계
    USceneComponent* AttachParent;           // 부모 Component
    TArray<USceneComponent*> AttachChildren; // 자식 Component들
    
    // World Transform 계산
    FTransform GetComponentTransform() const;
    FVector GetComponentLocation() const;
    FRotator GetComponentRotation() const;
};
```

#### 계층 구조 (Hierarchy)
```
Actor
└── RootComponent (SceneComponent)
    ├── ChildComponent1 (SceneComponent)
    │   └── GrandChildComponent1 (SceneComponent)
    ├── ChildComponent2 (SceneComponent)
    └── ChildComponent3 (SceneComponent)
```

#### Transform 계산 방식
- **Relative Transform**: 부모 Component 기준 상대 좌표
- **World Transform**: World 좌표계 기준 절대 좌표
- **계산 공식**: WorldTransform = ParentWorldTransform × RelativeTransform

```cpp
// Transform 계산 예시
FTransform WorldTransform = GetComponentTransform();
FVector WorldLocation = WorldTransform.GetLocation();
FRotator WorldRotation = WorldTransform.GetRotation().Rotator();
FVector WorldScale = WorldTransform.GetScale3D();
```

#### Attach/Detach 시스템
```cpp
// Component 연결
void AttachToComponent(
    USceneComponent* Parent,
    const FAttachmentTransformRules& AttachmentRules,
    FName SocketName = NAME_None
);

// Component 분리
void DetachFromComponent(
    const FDetachmentTransformRules& DetachmentRules
);

// Socket을 통한 연결 (SkeletalMeshComponent에서 주로 사용)
void AttachToSocket(
    USceneComponent* Parent,
    FName SocketName
);
```

#### Socket 시스템
- **SkeletalMeshComponent**에서 특정 뼈에 Component를 연결
- 캐릭터의 손에 무기, 머리에 모자 등을 연결할 때 사용

```cpp
// Socket 연결 예시
class AWeapon : public AActor {
    UPROPERTY(VisibleAnywhere)
    class UStaticMeshComponent* WeaponMesh;
    
    // 캐릭터의 손 Socket에 연결
    void AttachToHand(USkeletalMeshComponent* HandMesh) {
        WeaponMesh->AttachToComponent(
            HandMesh,
            FAttachmentTransformRules::SnapToTargetNotIncludingScale,
            TEXT("WeaponSocket")  // SkeletalMesh에서 정의된 Socket 이름
        );
    }
};
```

#### SceneComponent의 주요 메서드
```cpp
// 위치 관련
void SetRelativeLocation(const FVector& NewLocation);
void SetRelativeRotation(const FRotator& NewRotation);
void SetRelativeScale3D(const FVector& NewScale3D);

// World 좌표 관련
void SetWorldLocation(const FVector& NewLocation);
void SetWorldRotation(const FRotator& NewRotation);
void SetWorldScale3D(const FVector& NewScale3D);

// 이동 관련
void AddRelativeLocation(const FVector& DeltaLocation);
void AddRelativeRotation(const FRotator& DeltaRotation);
void AddWorldOffset(const FVector& DeltaLocation);

// 부모-자식 관계
void AttachToComponent(USceneComponent* Parent, ...);
void DetachFromComponent(...);
USceneComponent* GetAttachParent() const;
TArray<USceneComponent*> GetAttachChildren() const;
```

#### SceneComponent 사용 예시
```cpp
class APlayerCharacter : public APawn {
    // RootComponent (Capsule)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    class UCapsuleComponent* CapsuleComponent;
    
    // 캐릭터 메시 (Capsule에 연결)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    class USkeletalMeshComponent* MeshComponent;
    
    // 카메라 (Mesh의 Head Socket에 연결)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    class UCameraComponent* CameraComponent;
    
    // 무기 (Mesh의 Hand Socket에 연결)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    class UStaticMeshComponent* WeaponComponent;
    
    virtual void BeginPlay() override {
        Super::BeginPlay();
        
        // Mesh를 Capsule에 연결
        MeshComponent->AttachToComponent(
            CapsuleComponent,
            FAttachmentTransformRules::SnapToTargetNotIncludingScale
        );
        
        // 카메라를 Mesh의 Head Socket에 연결
        CameraComponent->AttachToComponent(
            MeshComponent,
            FAttachmentTransformRules::SnapToTargetNotIncludingScale,
            TEXT("HeadSocket")
        );
        
        // 무기를 Mesh의 Hand Socket에 연결
        WeaponComponent->AttachToComponent(
            MeshComponent,
            FAttachmentTransformRules::SnapToTargetNotIncludingScale,
            TEXT("HandSocket")
        );
    }
};
```

### 2. PrimitiveComponent
- SceneComponent를 상속
- **물리적 표현**을 가짐 (Collision, Physics)
- Mesh, StaticMesh, SkeletalMesh 등

### 3. ActorComponent
- **기능적 로직**을 담당
- Transform 정보 없음
- Health, Inventory, AI 등의 로직

## 실제 사용 예시

### 플레이어 캐릭터 구성
```cpp
class APlayerCharacter : public APawn {
    // RootComponent (위치 담당)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    class UCapsuleComponent* CapsuleComponent;
    
    // 시각적 표현
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    class USkeletalMeshComponent* MeshComponent;
    
    // 카메라
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    class UCameraComponent* CameraComponent;
    
    // 이동
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    class UCharacterMovementComponent* MovementComponent;
    
    // 입력
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    class UInputComponent* InputComponent;
    
    // 생명력
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    class UHealthComponent* HealthComponent;
};
```