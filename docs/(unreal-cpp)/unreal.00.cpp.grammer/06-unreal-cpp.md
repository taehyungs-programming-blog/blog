---
layout: default
title: "06. Interface"
parent: "(문법)"
grand_parent: "(Unreal C++ 🚀)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌎](https://github.com/Arthur880708/Unreal_Cpp_basic/tree/main/5.Interface)

* Interface를 두어 수업을 들을 수 있는 사람, 없는 사람을 구분해 보자.

```cpp
// This class does not need to be modified.
// 인터페이스를 생성하면 자동으로 아래가 생성된다.
UINTERFACE(MinimalAPI)
class ULessonInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 인터페이스 제작
 */
class UNREALINTERFACE_API ILessonInterface
{
	GENERATED_BODY()

public:
	virtual void DoLesson() // abstract - abstract로 구현해도 좋다
	{
		UE_LOG(LogTemp, Log, TEXT("수업에 입장합니다."));
	}
};
```

```cpp
// 사용법은 아주 쉽다
UCLASS()
class UNREALINTERFACE_API UTeacher : public UPerson, public ILessonInterface
{
	GENERATED_BODY()
public:
	UTeacher();

	virtual void DoLesson() override;
};
```

```cpp
void UMyGameInstance::Init()
{
	Super::Init();

	UE_LOG(LogTemp, Log, TEXT("============================"));
	TArray<UPerson*> Persons = { NewObject<UStudent>(), NewObject<UTeacher>(), NewObject<UStaff>() };
	for (const auto Person : Persons)
	{
		UE_LOG(LogTemp, Log, TEXT("구성원 이름 : %s"), *Person->GetName());
	}
	UE_LOG(LogTemp, Log, TEXT("============================"));

	for (const auto Person : Persons)
	{
        // 이런식으로 인터페이스를 활용해보자
		ILessonInterface* LessonInterface = Cast<ILessonInterface>(Person);
		if (LessonInterface)
		{
			UE_LOG(LogTemp, Log, TEXT("%s님은 수업에 참여할 수 있습니다."), *Person->GetName());
			LessonInterface->DoLesson();
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("%s님은 수업에 참여할 수 없습니다."), *Person->GetName());
		}
	}
	UE_LOG(LogTemp, Log, TEXT("============================"));
}
```

```cpp
UCLASS()
class UNREALINTERFACE_API UPerson : public UObject
{
	GENERATED_BODY()
public:
	UPerson();

    // (TIPS) FORCEINLINE을 통해 inline을 권유할 수 있음(강제는 아님)
	FORCEINLINE FString& GetName() { return Name; }
	FORCEINLINE void SetName(const FString& InName) { Name = InName; }

protected:
	UPROPERTY()
	FString Name;
};
```