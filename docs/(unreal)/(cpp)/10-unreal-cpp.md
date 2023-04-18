---
layout: default
title: "10. Memeory Management"
parent: "(Unreal C++)"
grand_parent: "(Unreal 🚀)"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌎](https://github.com/Arthur880708/Unreal_Cpp_basic/tree/main/9.Memory)

```cpp
UCLASS()
class UNREALMEMORY_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:

	virtual void Init() override;

	virtual void Shutdown() override;


private:

    // UPROPERTY()를 안넣은경우
	TObjectPtr<class UStudent> NonPropStudent;

	UPROPERTY()
	TObjectPtr<class UStudent> PropStudent;

    // UPROPERTY()를 안넣은경우
	TArray<TObjectPtr<class UStudent>> NonPropStudents;

	UPROPERTY()
	TArray<TObjectPtr<class UStudent>> PropStudents;


	class FStudentManager* StudentManager = nullptr;
};
```

```cpp
void CheckUObjectIsValid(const UObject* InObject, const FString& InTag)
{
	if (InObject->IsValidLowLevel())
	{
		UE_LOG(LogTemp, Log, TEXT("[%s] 유효한 언리얼 오브젝트"), *InTag);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("[%s] 유효하지 않은 언리얼 오브젝트"), *InTag);
	}
}

void CheckUObjectIsNull(const UObject* InObject, const FString& InTag)
{
	if (nullptr == InObject)
	{
		UE_LOG(LogTemp, Log, TEXT("[%s] 널 포인터 언리얼 오브젝트"), *InTag);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("[%s] 널 포인터가 아닌 언리얼 오브젝트"), *InTag);
	}
}

void UMyGameInstance::Init()
{
	Super::Init();

	NonPropStudent = NewObject<UStudent>();
	PropStudent = NewObject<UStudent>();

	NonPropStudents.Add(NewObject<UStudent>());
	PropStudents.Add(NewObject<UStudent>());

	StudentManager = new FStudentManager(NewObject<UStudent>());
}

void UMyGameInstance::Shutdown()
{
	Super::Shutdown();

	const UStudent* StudentInManager = StudentManager->GetStudent();

	delete StudentManager;
	StudentManager = nullptr;

	CheckUObjectIsNull(StudentInManager, TEXT("StudentInManager"));
    // LogTemp: [StudentInManager] 널 포인터가 아닌 언리얼 오브젝트
	CheckUObjectIsValid(StudentInManager, TEXT("StudentInManager"));
    // LogTemp: [StudentInManager] 유효한 언리얼 오브젝트

	CheckUObjectIsNull(NonPropStudent, TEXT("NonPropStudent"));
    // LogTemp: [NonPropStudent] 널 포인터가 아닌 언리얼 오브젝트
	CheckUObjectIsValid(NonPropStudent, TEXT("NonPropStudent"));
    // LogTemp: [NonPropStudent] 유효하지 않은 언리얼 오브젝트

	CheckUObjectIsNull(PropStudent, TEXT("PropStudent"));
    // LogTemp: [PropStudent] 널 포인터가 아닌 언리얼 오브젝트
	CheckUObjectIsValid(PropStudent, TEXT("PropStudent"));
    // LogTemp: [PropStudent] 유효한 언리얼 오브젝트

	CheckUObjectIsNull(NonPropStudents[0], TEXT("NonPropStudents"));
    // LogTemp: [NonPropStudents] 널 포인터가 아닌 언리얼 오브젝트
	CheckUObjectIsValid(NonPropStudents[0], TEXT("NonPropStudents"));
    // LogTemp: [NonPropStudents] 유효하지 않은 언리얼 오브젝트

	CheckUObjectIsNull(PropStudents[0], TEXT("PropStudents"));
    // LogTemp: [PropStudents] 널 포인터가 아닌 언리얼 오브젝트
	CheckUObjectIsValid(PropStudents[0], TEXT("PropStudents"));
    // LogTemp: [PropStudents] 유효한 언리얼 오브젝트
}
```