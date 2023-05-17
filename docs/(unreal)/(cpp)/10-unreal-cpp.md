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

* Unreal은 GC가 자동으로 돌아간다
* 따라서 내가 삭제하지 않더라도 자동으로 메모리가 정리돼어 버리는데 그걸 막기 위해 `UPROPERTY()`가 필요하다
* 추가적으로 궁금한 점은 그럼 삭제는 어떻게 하냐이다.
	* 참조가 다 끊어지거나
	* `Destroy()` 혹은 `MarkPendingKill()`과 같은 함수가 호출되었을 때 이다.

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