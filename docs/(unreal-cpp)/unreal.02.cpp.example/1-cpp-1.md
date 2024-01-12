---
layout: default
title: "01. 캐릭터 세팅"
parent: "(Example - RPG Action)"
grand_parent: "(Unreal C++ 🚀)"
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
* 필요 패키지(일단은 다 필요하진 않음)
	* Stylized Character Kit: Casual 01
	* Adventure Character
	* Pirate
	* Infinity Blade: Weapons
	* FX Variety Pack
	* Realistic Starter VFX Pack Vol 2
	* Niagara Footstep VFX
	* Human Vocalizations
	* Interface & Item Sounds Pack
	* (아래 애니메이션은 압축해서 제공예정)
	* Paragon: Gideon
	* Paragon: Greystone
	* Paragon: Kwang
	* Paragon: Murdock

---

## Tips

* Visual Studio 버전설정
	* Edit -> Editor Preferences -> General -> Source Code
* 특정 Material의 Find Reference
	* Asset -> Reference Viewer

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_1/1-cpp-1-13.png"/>
</p>

---

## Project 생성

* Third Person, C++로 만들자
* 필요 패키지를 추가하자
	* Stylized Character Kit: Casual 01
	* Adventure Character
	* Infinity Blade: Weapons
	* Pirate
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

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_1/1-cpp-1-1.png"/>
</p>

* 요거만 수정해주면 된다는 말

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_1/1-cpp-1-14.png"/>
</p>

---

## 모든 애니메이션을 공통으로 쓰기위해 IK Rig을 만들자

* 혹시나 해서, pelvis는 항상 Root로 둬야한다
* 하나만 예시로 들어보자면 ...
	* UE4 Mannequin의 Skeletal Mesh를 IK Rig을 만들어보자(우클릭 후 Animation -> IK Rig)
	* 아래와 같은 화면이 나타나고 IK Rig을 만들어 주면 된다
	* 만드는 방법은 원하는 Rig 선택 후 오른쪽의 Add New Chain을 해주면 됨
* 다음의 IK Rig을 생성하자 (Rig은 어차피 테스트 용도이기에 최대한 간단하게 만들어 둠)
	* IK_UE4_Mannequin
	* IKRig_Basic4
	* IKRig_Basic5
	* IKRig_Man
	* IKRig_Pirate
	* IKRig_ProjectRPG
* 모두 리타게터도 만들어야 하며 타겟은 ProjectRPG

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_1/1-cpp-1-2.png"/>
</p>

* 이렇게 모든 Skeletal Mesh의 IK Rig을 생성 후
* IK Retargetor를 만들어 애니메이션이 잘 동작하나 확인한다
* 헷갈리는 용어들 정리
	* **IK Rig** - 해당 Skeleton Mesh의 뼈대를 정의
	* **IK Retargeter** - 정의된 뼈대를 비교하여 애니메이션이 잘 동작하나 확인 및 애니메이션을 만들어준다
	* **Retarget animation Asset** - Retargeter를 바탕으로 애니메이션 생성
* 뭔 소린지?
	* 예를들어 A Skeleton Mesh로 만들어진 Animation이 있다고 가정하자
	* 이 Animation을 B Skeleton Mesh에 넣고싶다
	* 그럼 IK Retargeter를 A에서 만들어 Target을 B로 지정한다
	* Retargeter에서 B Target으로 테스트해도 이상이 없을시 Animation으 B로 Retarget animation Asset하여도 이상없이 동작하게된다.

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

---

### Animation Blue Print을 만져보자!

* 다른 부분은 Blueprint를 보면 되고
* State Alias가 뭔지 알아야한다.
  * 간단히 특정 State일때 Alias가 바로 호출된다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_1/1-cpp-1-9.png"/>
</p>

* 예를들어 To Falling Alias는 Locomotion, Land일때 호출되며
* 그럼 매번 Jump를 해야하는거 아닌가 싶지만 그렇진 않다 State가 호출되기 위해선 또 조건검사를 하기 때문이다.

* 설명이 부족한거 같아 추가설명함.
* **State Machine** - 함수라 생각하면되고 단, Animation을 리턴해준다

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_1/1-cpp-1-15.png"/>
</p>

* State Machine내부로 들어가보자

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_1/1-cpp-1-16.png"/>
</p>

* 여러 State가 존재 하며 State내부에는 Animation을 리턴하게 되어있다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_1/1-cpp-1-17.png"/>
</p>

* 단, Animation을 바로 리턴하지않고 cache에 임시 저장할 수도 있다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_1/1-cpp-1-18.png"/>
</p>

* 특이한 점은 State Machine이후에 Slot DefaultSlot을 거친다는 것인데

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_1/1-cpp-1-19.png"/>
</p>

* 코드상에서 Animation Montage를 출력할 경우 이 State에서 그리게 해준다

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

* 음? 이게 된다고?
* 나도 처음엔 신기했음, 해당 Mesh로 가보면 Mesh의 Skeleton이 내가 사용하는 Mesh의 Skeleton과 일치하고
* Skeleton Tree로 가보면 Vertice가 연관됨을 알려준다

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_1/1-cpp-1-20.png"/>
</p>

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_1/1-cpp-1-21.png"/>
</p>