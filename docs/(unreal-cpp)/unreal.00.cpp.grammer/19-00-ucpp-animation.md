---
layout: default
title: "19. 애니메이션 기초"
parent: "(문법)"
grand_parent: "(Unreal C++ 🚀)"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌟](https://github.com/Arthur880708/UnrealEngineGrammer/tree/2)

---

* 우선 AnimInstance를 만들어 줘야한다.

```cpp
UCLASS()
class R1_API UR1AnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UR1AnimInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<class AR1Player> Character;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<class UCharacterMovementComponent> MovementComponent;

protected:
	UPROPERTY(BlueprintReadOnly)
	FVector Velocity = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly)
	float GroundSpeed = 0.f;

	UPROPERTY(BlueprintReadOnly)
	bool bShouldMove = false;

	UPROPERTY(BlueprintReadOnly)
	bool bIsFalling = false;
};
```

* 다음은 Blueprint도 생성

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/grammer/ucpp0-19-1.png"/>
</p>

* 위에서 생성한 BP는 Player에서 사용한다고 선택하게 된다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/grammer/ucpp0-19-2.png"/>
</p>

* 보통여기서 궁금한 점은 **Locomotion은 어떻게 쓰나?**
* 우선, 기본은 Output Pose에 어떤 애니메이션을 넣으면 그 애니메이션으로 동작하게 된다.
    * 그럼 어떻게 Output Pose에 애니메이션을 넣을까?
* Locomotion은 **State Machine**이다.
    * 우 클릭 후 State Machine 생성
* CachedLocomotion은 임시 저장이다.


