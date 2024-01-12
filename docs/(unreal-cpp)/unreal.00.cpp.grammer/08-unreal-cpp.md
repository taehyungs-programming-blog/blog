---
layout: default
title: "08. Delegate"
parent: "(문법)"
grand_parent: "(Unreal C++ 🚀)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌎](https://github.com/Arthur880708/Unreal_Cpp_basic/tree/main/7.Delegate)

* 구독후 발행된 정보를 받는 패턴을 만들어 보려한다.

* 델리게이트 선언 매크로
    * `DECLARE_DELEGATE` - 일대일 형태로 C++만 지원한다면 공란으로 둔다
    * `DECLARE_MULTICAST` - 일대다 형태로 C++만 지원한다면 MULTICAST 선언
    * `DECLARE_DYNAMIC` - 일대일 형태로 블루프린트를 지원한다며 DYNAMIC 선언
    * `DECLARE_DYNAMIC_MULTICAST` - 일대다 형태로 블루프린트를 지원한다면 DYNAMIC, MULTICAST 조합
* 함수 정보도 같이 넘긴다
    * 인자가 없을경우 공란
    * `OneParam` - 인자가 하나고 반환값이 없는 경우
    * `ThreeParams` - 인자가 세 개고 반환값이 없는 경우(최대 9개까지 지원)

```cpp
DECLARE_MULTICAST_DELEGATE_TwoParams(FCourseInfoOnChangedSignature, const FString&, const FString&);

/**
 * 
 */
UCLASS()
class UNREALDELEGATE_API UCourseInfo : public UObject
{
	GENERATED_BODY()
public:
	UCourseInfo();

    // OnChanged를 델리게이터로 사용할 예정
	FCourseInfoOnChangedSignature OnChanged;

	void ChangeCourseInfo(const FString& InSchoolName, const FString& InNewContents);

private:
	FString Contents;
};
```

```cpp
void UCourseInfo::ChangeCourseInfo(const FString& InSchoolName, const FString& InNewContents)
{
	Contents = InNewContents;

	UE_LOG(LogTemp, Log, TEXT("[CourseInfo] 학사 정보가 변경되어 알림을 발송합니다."));
    // ChangeCourseInfo호출 시 등록된 OnChanged로 broadcast를 보낸다
	OnChanged.Broadcast(InSchoolName, Contents);
}
```

* 등록은 어디서 하나?

```cpp
UCLASS()
class UNREALDELEGATE_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	UMyGameInstance();

	virtual void Init() override;

private:
	UPROPERTY()
	TObjectPtr<class UCourseInfo> CourseInfo;

	UPROPERTY()
	FString SchoolName;
};
```

```cpp
void UMyGameInstance::Init()
{
	Super::Init();

	CourseInfo = NewObject<UCourseInfo>(this);

	UE_LOG(LogTemp, Log, TEXT("============================"));

	UStudent* Student1 = NewObject<UStudent>();
	Student1->SetName(TEXT("학생1"));
	UStudent* Student2 = NewObject<UStudent>();
	Student2->SetName(TEXT("학생2"));
	UStudent* Student3 = NewObject<UStudent>();
	Student3->SetName(TEXT("학생3"));

	CourseInfo->OnChanged.AddUObject(Student1, &UStudent::GetNotification);
	CourseInfo->OnChanged.AddUObject(Student2, &UStudent::GetNotification);
	CourseInfo->OnChanged.AddUObject(Student3, &UStudent::GetNotification);

	CourseInfo->ChangeCourseInfo(SchoolName, TEXT("변경된 학사 정보"));

	UE_LOG(LogTemp, Log, TEXT("============================"));
}
```

```cpp
void UStudent::GetNotification(const FString& School, const FString& NewCourseInfo)
{
	UE_LOG(LogTemp, Log, TEXT("[Student] %s님이 %s로부터 받은 메시지 : %s"), *Name, *School, *NewCourseInfo);
}
```