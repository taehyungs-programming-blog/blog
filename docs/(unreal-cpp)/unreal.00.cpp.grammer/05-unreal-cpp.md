---
layout: default
title: "05. Reflection System"
parent: "(문법)"
grand_parent: "(Unreal C++ 🚀)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌎](https://github.com/Arthur880708/Unreal_Cpp_basic/tree/main/4.Reflection)

## Reflection System

* Unreal Property System이라고 불리기도 한다.
* **Reflection System** - Run time에 자기 자신을 조사하는 기능
    * `*.generated.h`에 자동 포함된다.

```cpp
UCLASS()
class OBJECTREFLECTION_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()	
public:
	UMyGameInstance();

	virtual void Init() override;

private:
	UPROPERTY()         // Reflection을 설치한다고 생각하자
	FString SchoolName;
};
```

```cpp
// 자신의 헤더가 가장위에 와야함(주의)
#include "MyGameInstance.h"
#include "Student.h"
#include "Teacher.h"

UMyGameInstance::UMyGameInstance()
{
    // 헤더에서 기본값을 선언하지 않음을 기억
        // 한글을 쓰기때문에 .cpp를 UTF8로 저장할 것
    // (주의!!!) 만약 기본값을 수정할 경우 UE5 에디터를 한번 껏다 키자(마치 헤더 수정처럼)
	SchoolName = TEXT("기본학교");
}

void UMyGameInstance::Init()
{
	Super::Init();

	UE_LOG(LogTemp, Log, TEXT("============================="));
	UClass* ClassRuntime = GetClass();
	UClass* ClassCompile = UMyGameInstance::StaticClass();
    // 같은 클래스인지 확인
    check(ClassRuntime == ClassCompile);    // debug모드에서 false일 경우 crash가 난다(자주쓰자)

	//ensure(ClassRuntime != ClassCompile); // crash까지는 안나고 log로 보여준다
	//ensureMsgf(ClassRuntime != ClassCompile, TEXT("일부러 에러를 발생시킨 코드"));    // 역시 crash까지 안나고 메시지로 남겨준다

    // ---- ---- ---- ----

	UE_LOG(LogTemp, Log, TEXT("학교를 담당하는 클래스 이름 : %s"), *ClassRuntime->GetName());

	SchoolName = TEXT("청강문화산업대학교");

	UE_LOG(LogTemp, Log, TEXT("학교 이름 : %s"), *SchoolName);

    // GetDefaultObject - CDO(Class Default Object)를 출력, 변경전 이름을 출력해 줄것이다.
	UE_LOG(LogTemp, Log, TEXT("학교 이름 기본값 : %s"), *GetClass()->GetDefaultObject<UMyGameInstance>()->SchoolName);

	UE_LOG(LogTemp, Log, TEXT("============================="));

	UStudent* Student = NewObject<UStudent>();
	UTeacher* Teacher = NewObject<UTeacher>();
	Student->SetName(TEXT("학생1"));
	UE_LOG(LogTemp, Log, TEXT("새로운 학생 이름 %s"), *Student->GetName());

	FString CurrentTeacherName;
	FString NewTeacherName(TEXT("이득우"));
	FProperty* NameProp = UTeacher::StaticClass()->FindPropertyByName(TEXT("Name"));
	if (NameProp)
	{
		NameProp->GetValue_InContainer(Teacher, &CurrentTeacherName);
		UE_LOG(LogTemp, Log, TEXT("현재 선생님 이름 %s"), *CurrentTeacherName);

		NameProp->SetValue_InContainer(Teacher, &NewTeacherName);
		UE_LOG(LogTemp, Log, TEXT("새로운 선생님 이름 %s"), *Teacher->GetName());
	}

	UE_LOG(LogTemp, Log, TEXT("============================="));

	Student->DoLesson();
    // Reflection 설치가 되었기에 가능함.
	UFunction* DoLessonFunc = Teacher->GetClass()->FindFunctionByName(TEXT("DoLesson"));
	if (DoLessonFunc)
	{
		Teacher->ProcessEvent(DoLessonFunc, nullptr);
	}

	UE_LOG(LogTemp, Log, TEXT("============================="));
}
```

```cpp
UCLASS()
class OBJECTREFLECTION_API UPerson : public UObject
{
	GENERATED_BODY()
public:
	UPerson();

	UFUNCTION()                 // Function 역시 Reflection 설치
	virtual void DoLesson();

	const FString& GetName() const;
	void SetName(const FString& InName);

protected:
	UPROPERTY()
	FString Name;

	UPROPERTY()
	int32 Year;

private:
};
```

```cpp
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Person.h"

// Include시에 항상 *.generated.h는 가장아래 와야한다
#include "Student.generated.h"

/**
 * 
 */
UCLASS()
class OBJECTREFLECTION_API UStudent : public UPerson
{
	GENERATED_BODY()
public:
	UStudent();

	virtual void DoLesson() override;

private:
	UPROPERTY()
	int32 Id;

};
```

```
LogTemp: =============================
LogTemp: 학교를 담당하는 클래스 이름 : MyGameInstance
LogTemp: 학교 이름 : 청강문화산업대학교
LogTemp: 학교 이름 기본값 : 기본학교
LogTemp: =============================
LogTemp: 새로운 학생 이름 학생1
LogTemp: 현재 선생님 이름 이선생
LogTemp: 새로운 선생님 이름 이득우
LogTemp: =============================
LogTemp: 학생1님이 수업에 참여합니다.
LogTemp: 1학년 1번 학생1님이 수업을 듣습니다.
LogTemp: 이득우님이 수업에 참여합니다.
LogTemp: 3년차 선생님 이득우님이 수업을 강의합니다.
LogTemp: =============================
```