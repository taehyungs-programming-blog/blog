---
layout: default
title: "25. CDO(ClassDefaultObject)"
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

## CDO란 무엇인가?

* **CDO(Class Default Object)**는 Unreal Engine에서 클래스의 기본값을 담는 객체
* 각 클래스마다 하나의 CDO가 생성되며, 이 객체는 해당 클래스의 모든 인스턴스가 참조하는 기본값들을 저장

### 간단한 예시
- 전사 클래스를 만들면 → 전사용 CDO 생성
- 마법사 클래스를 만들면 → 마법사용 CDO 생성
- 각 CDO는 해당 클래스의 기본 HP, MP, 공격력 등의 값을 저장

## 왜 CDO가 필요한가?

### 생성자에 값을 박으면 안 되나?

```cpp
// 나쁜 예시 - 생성자에 하드코딩
AWarrior::AWarrior()
{
    Health = 100;        // 이 값들을 변경하려면?
    Mana = 50;           // 코드를 수정하고
    AttackPower = 25;    // 다시 컴파일해야 함!
}
```

**문제점:**
1. 값을 변경하려면 코드 수정 후 재컴파일 필요
2. 디자이너가 게임 밸런스를 조정하기 어려움
3. 런타임에 접근/수정 불가능

### CDO의 해결책

CDO를 사용하면:
- **에디터에서 간단하게 수정 가능**
- **런타임에 접근/수정 가능**
- **재컴파일 없이 값 변경 가능**

## CDO 사용 예시

### 1. 기본 클래스 정의

```cpp
// Warrior.h
UCLASS(BlueprintType, Blueprintable)
class MYGAME_API AWarrior : public APawn
{
    GENERATED_BODY()

public:
    AWarrior();

    // CDO에서 설정할 수 있는 기본값들
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float DefaultHealth;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float DefaultMana;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float DefaultAttackPower;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float DefaultMoveSpeed;

    // 런타임에서 사용할 현재 값들
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
    float CurrentHealth;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
    float CurrentMana;

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;
};
```

### 2. 생성자에서 CDO 기본값 설정

```cpp
// Warrior.cpp
#include "Warrior.h"

AWarrior::AWarrior()
{
    PrimaryActorTick.bCanEverTick = true;

    // CDO의 기본값 설정
    // 이 값들은 에디터에서 수정 가능
    DefaultHealth = 100.0f;
    DefaultMana = 50.0f;
    DefaultAttackPower = 25.0f;
    DefaultMoveSpeed = 300.0f;

    // 현재 값들은 BeginPlay에서 초기화
    CurrentHealth = 0.0f;
    CurrentMana = 0.0f;
}

void AWarrior::BeginPlay()
{
    Super::BeginPlay();
    
    // CDO에서 기본값을 가져와서 현재 값으로 설정
    CurrentHealth = DefaultHealth;
    CurrentMana = DefaultMana;
}
```

### 3. 코드에서 CDO에 접근하는 방법

```cpp
// 다른 클래스에서 Warrior의 CDO에 접근
void AGameManager::SetupWarriorDefaults()
{
    // 1. 클래스에서 CDO 가져오기
    AWarrior* WarriorCDO = AWarrior::StaticClass()->GetDefaultObject<AWarrior>();
    
    if (WarriorCDO)
    {
        // CDO의 기본값 읽기
        float DefaultHP = WarriorCDO->DefaultHealth;
        float DefaultMP = WarriorCDO->DefaultMana;
        
        UE_LOG(LogTemp, Warning, TEXT("Warrior Default HP: %f"), DefaultHP);
        UE_LOG(LogTemp, Warning, TEXT("Warrior Default MP: %f"), DefaultMP);
        
        // 런타임에 CDO 값 수정 (주의: 모든 인스턴스에 영향)
        WarriorCDO->DefaultHealth = 150.0f;  // HP를 150으로 변경
    }
}

// Blueprint에서도 CDO 접근 가능
UFUNCTION(BlueprintCallable, Category = "Game")
void AGameManager::ModifyWarriorDefaults(float NewHealth, float NewMana)
{
    AWarrior* WarriorCDO = AWarrior::StaticClass()->GetDefaultObject<AWarrior>();
    
    if (WarriorCDO)
    {
        WarriorCDO->DefaultHealth = NewHealth;
        WarriorCDO->DefaultMana = NewMana;
        
        UE_LOG(LogTemp, Warning, TEXT("Warrior defaults updated!"));
    }
}
```

### 4. Blueprint에서 CDO 활용

```cpp
// Blueprint에서 호출 가능한 함수들
UCLASS(BlueprintType, Blueprintable)
class MYGAME_API AWarrior : public APawn
{
    GENERATED_BODY()

public:
    // Blueprint에서 현재 CDO 값 가져오기
    UFUNCTION(BlueprintCallable, Category = "Stats")
    float GetDefaultHealth() const { return DefaultHealth; }

    UFUNCTION(BlueprintCallable, Category = "Stats")
    float GetDefaultMana() const { return DefaultMana; }

    // Blueprint에서 CDO 값 수정 (런타임)
    UFUNCTION(BlueprintCallable, Category = "Stats")
    void SetDefaultHealth(float NewHealth) { DefaultHealth = NewHealth; }

    UFUNCTION(BlueprintCallable, Category = "Stats")
    void SetDefaultMana(float NewMana) { DefaultMana = NewMana; }

    // 현재 값과 기본값 비교
    UFUNCTION(BlueprintCallable, Category = "Stats")
    bool IsAtFullHealth() const { return CurrentHealth >= DefaultHealth; }

    UFUNCTION(BlueprintCallable, Category = "Stats")
    float GetHealthPercentage() const 
    { 
        return DefaultHealth > 0 ? (CurrentHealth / DefaultHealth) : 0.0f; 
    }
};
```

## CDO의 주요 장점

### 1. 에디터 통합
- **Details 패널**에서 직접 수정 가능
- **Blueprint 에디터**에서 시각적으로 조정
- **재컴파일 없이** 값 변경 가능

### 2. 런타임 유연성
```cpp
// 게임 중에 밸런스 조정
void AWarrior::ApplyDifficultyModifier(float DifficultyMultiplier)
{
    // CDO 값에 기반하여 현재 값 조정
    CurrentHealth = DefaultHealth * DifficultyMultiplier;
    CurrentMana = DefaultMana * DifficultyMultiplier;
}
```

### 3. 상속과 오버라이드
```cpp
// 상속받은 클래스에서 CDO 값 오버라이드
ABerserker::ABerserker()
{
    // 전사의 기본값을 오버라이드
    DefaultHealth = 150.0f;    // 더 높은 HP
    DefaultMana = 20.0f;       // 더 낮은 MP
    DefaultAttackPower = 40.0f; // 더 높은 공격력
}
```

## 실제 사용 시나리오

### 1. 게임 밸런싱
```cpp
// 게임 디자이너가 에디터에서 쉽게 조정
// - 전사 HP: 100 → 120
// - 마법사 MP: 80 → 100
// - 궁수 공격력: 30 → 35
```

### 2. 다국어 지원
```cpp
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Localization")
FText CharacterName;

// CDO에서 각 언어별로 다른 이름 설정 가능
```

### 3. 플랫폼별 최적화
```cpp
// 모바일과 PC에서 다른 기본값 사용
#if PLATFORM_ANDROID || PLATFORM_IOS
    DefaultHealth = 80.0f;  // 모바일: 낮은 HP
#else
    DefaultHealth = 100.0f; // PC: 기본 HP
#endif
```

## 주의사항

### 1. CDO 수정의 영향
```cpp
// 주의: CDO 수정은 모든 인스턴스에 영향
AWarrior* CDO = AWarrior::StaticClass()->GetDefaultObject<AWarrior>();
CDO->DefaultHealth = 200.0f;  // 모든 Warrior 인스턴스에 영향!
```

### 2. 메모리 관리
- CDO는 **자동으로 관리**됨
- **수동으로 삭제하면 안 됨**
- **가비지 컬렉션**에 의해 관리

### 3. 성능 고려사항
```cpp
// CDO 접근은 빠르지만, 너무 자주 호출하지 말 것
// 캐싱을 고려하세요
static AWarrior* CachedWarriorCDO = nullptr;

AWarrior* GetWarriorCDO()
{
    if (!CachedWarriorCDO)
    {
        CachedWarriorCDO = AWarrior::StaticClass()->GetDefaultObject<AWarrior>();
    }
    return CachedWarriorCDO;
}
```
