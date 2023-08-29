---
layout: default
title: "01. 캐릭터 세팅"
parent: "(Unreal C++ RPG Action만 구현)"
grand_parent: "(Unreal 🚀)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* 다운 받아야할 패키지가 많아서 될 수 있으면 코드를 Clone 받자

* [Get Code 🌎](https://github.com/Arthur880708/Unreal_RPG_Tutorial_1)

* 환경
    * UE 5.0.3
    * Visual Studio 2022
* 필요 패키지
	* Stylized Character Kit: Casual 01
	* Adventure Character
	* Pirate
	* Infinity Blade: Weapons
	* FX Variety Pack
	* Realistic Starter VFX Pack Vol 2
	* Niagara Footstep VFX
	* Human Vocalizations
	* Interface & Item Sounds Pack
	* Paragon: Gideon
	* Paragon: Greystone
	* Paragon: Kwang
	* Paragon: Murdock

---

## Project 생성

* Third Person, C++로 만들자
* 필요 패키지를 추가하자
	* Stylized Character Kit: Casual 01
	* Adventure Character
	* Infinity Blade: Weapons
* 애니메이션은 별도로 다운이 필요
	* [여기](https://downapi.cafe.naver.com/v1.0/cafes/article/file/ca135895-2c2d-11ee-9cd8-0050568dd5de/download)
	* Import시 적당히 마네퀸 정도로 UE4 Skeletal Mesh를 정하면 된다.(어차피 나중에 바꿀예정)

---

## 패키지 Import시 주의사항, 애니메이션 Import 하기

* EnbaleRootMotion과 Force Root Lock을 Enable시켜놔야 한다
* 애니메이션 우 클릭 후 -> Asset Actions -> Bulk Edit via Property Matrix ...
* 우측에 RootMotion -> Enable RootMion, Force Root Lock 클릭

---

## 캐릭터에 사용될 Skeletal Mesh 준비

* 아래와 같이 Skeletal Mesh에 Material만 수정해 여러개를 만들어 두자
	* 수정방법은 사용되는 Mesh의 Color만 수정하면 됨

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_1/1-cpp-1-1.png"/>
</p>

<사진>

---

## 모든 애니메이션을 공통으로 쓰기위해 IK Rig을 만들자

* 혹시나 해서, prlvis는 항상 Root로 둬야한다

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_1/1-cpp-1-2.png"/>
</p>

* 이렇게 모든 Skeletal Mesh의 IK Rig을 생성 후
* IK Retargetor를 만들어 애니메이션이 잘 동작하나 확인한다

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_1/1-cpp-1-3.png"/>
</p>

* 애니메이션 우 클릭 후 Retarget Animation Assets -> Retarget animation Asset

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_1/1-cpp-1-4.png"/>
</p>

* 여기까지하면 애니메이션을 쓸 준빈 완료

---

## 달리기 처리할 Blender Space 1D하나 추가

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_1/1-cpp-1-5.png"/>
</p>

---

## AnimInstance 만들기

* C++ -> AnimInstance로 하나 만들자

```cpp
UCLASS()
class PROJECTRPG_API UProjectRPGAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pawn", meta = (AllowPrivateAccess = true))
	class ACharacter* Character;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pawn", meta = (AllowPrivateAccess = true))
	class UCharacterMovementComponent* MovementComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pawn", meta = (AllowPrivateAccess = true))
	FVector Velocity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pawn", meta = (AllowPrivateAccess = true))
	float GroundSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pawn", meta = (AllowPrivateAccess = true))
	bool bShouldMove;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pawn", meta = (AllowPrivateAccess = true))
	bool bIsFalling;

private:

};
```

* 이제 이 AnimInstance를 기반으로 Blueprint를 하나 만들자

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_1/1-cpp-1-6.png"/>
</p>

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_1/1-cpp-1-7.png"/>
</p>

* Tip) C++에서 만든 변수가 안보인다면?
  * 설정 -> Show Inherited Variables

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_1/1-cpp-1-8.png"/>
</p>

### Animation Blue Print을 만져보자!

* 다른 부분은 Blueprint를 보면 되고
* State Alias가 뭔지 알아야한다.
  * 간단히 특정 State일때 Alias가 바로 호출된다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_1/1-cpp-1-9.png"/>
</p>

* 예를들어 To Falling Alias는 Locomotion, Land일때 호출되며
* 그럼 매번 Jump를 해야하는거 아닌가 싶지만 그렇진 않다 State가 호출되기 위해선 또 조건검사를 하기 때문이다.

---

## GameMode도 Blueprint로 만들자

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_1/1-cpp-1-10.png"/>
</p>

---

## Item에 따라 파츠가 변경되게 해보자

* 파츠 저장을 위해 Character 소스코드 수정이 필요하다

```cpp
class AProjectRPGCharacter : public ACharacter
{
	GENERATED_BODY()

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Parts", meta = (AllowPrivateAccess = true))
	class USkeletalMeshComponent* UpperBody;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Parts", meta = (AllowPrivateAccess = true))
	class USkeletalMeshComponent* LowerBody;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Parts", meta = (AllowPrivateAccess = true))
	class USkeletalMeshComponent* Hand;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Parts", meta = (AllowPrivateAccess = true))
	class USkeletalMeshComponent* Hair;


  // 파츠를 담을 배열
	UPROPERTY(EditDefaultsOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	TArray<class USkeletalMesh*> UpperBodys;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory", meta = (AllowPrivateAccess = true))
	TArray<class USkeletalMesh*> LowerBodys;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	TArray<class USkeletalMesh*> Hairs;


public:
	TArray<class USkeletalMesh*> GetUpperBodys() const;

	TArray<class USkeletalMesh*> GetLowerBodys() const;

	TArray<class USkeletalMesh*> GetHairs() const;

  // ...
```

* Mesh아래 각 파츠를 붙이고(코드확인) blueprint를 보자

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_1/1-cpp-1-11.png"/>
</p>

* 그런데 Blueprint에서 각 파츠에 Mesh를 넣으면 마치 정렬이 안된거 처럼 보이게 되는데
* Set Master Pose Component의 호출이 한 번 필요하다

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_1/1-cpp-1-12.png"/>
</p>

* 이제 파츠를 랜덤으로 호출해 보자

```cpp
void AProjectRPGCharacter::OnChangeParts()
{
	int32 RandomIndex = FMath::RandRange(0, GetUpperBodys().Num() - 1);
	UpperBody->SetSkeletalMesh(GetUpperBodys()[RandomIndex]);

	RandomIndex = FMath::RandRange(0, GetLowerBodys().Num() - 1);
	LowerBody->SetSkeletalMesh(GetLowerBodys()[RandomIndex]);

	RandomIndex = FMath::RandRange(0, GetHairs().Num() - 1);
	Hair->SetSkeletalMesh(GetHairs()[RandomIndex]);
}
```