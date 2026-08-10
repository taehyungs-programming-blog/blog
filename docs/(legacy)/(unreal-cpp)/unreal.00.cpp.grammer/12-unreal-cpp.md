---
layout: default
title: "12. 패키지"
parent: "(문법)"
grand_parent: "(Unreal C++ 🚀)"
great_grand_parent: "Legacy Archive"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌎](https://github.com/Arthur880708/Unreal_Cpp_basic/tree/main/10.Serialization)

## Unreal Object Package

* 단일 언리얼 오브젝트가 가진 정보는 저장할 수 있지만, 오브젝트의 조합이라면 ?
    * 저장된 언리얼 오브젝트 데이터를 효과적으로 찾고 관리하는 방법은?
    * 복합한 계층 구조를 가진 언리얼 오브젝트를 효과적으로 저장/로드 방법이 필요하다
* Unreal Object Package(`UPackage`)의 등장!

---

## Package와 Asset

* **언리얼 오브젝트 패키지**는 다수의 언리얼 오브젝트를 포장하는데 사용된다.
* 언리얼 오브젝트 패키지의 서브 오브젝트를 **애셋**이라고 하며 에디터에서 확인이 가능하다.
* 애셋은 다시 서브 오브젝트를 가질 수 있으며, 모두 언리얼 오브젝트 패키지에 포함된다. (단, 에디터에는 노출되지 않음)

---

```cpp
const FString UMyGameInstance::PackageName = TEXT("/Game/Student");
const FString UMyGameInstance::AssetName = TEXT("TopStudent");

// ...

void UMyGameInstance::SaveStudentPackage() const
{
	UPackage* StudentPackage = ::LoadPackage(nullptr, *PackageName, LOAD_None);
	if (StudentPackage)
	{
		StudentPackage->FullyLoad();
	}

	StudentPackage = CreatePackage(*PackageName);
	EObjectFlags ObjectFlag = RF_Public | RF_Standalone;

	// 기존의 Object생성과 조금 다르다
	UStudent* TopStudent = NewObject<UStudent>(StudentPackage,  // 이 Package에 저장해 달라
                                    UStudent::StaticClass(),    // Class 정보
                                    *AssetName,                 // Asset 이름 (고유 이름 정보)
                                    ObjectFlag                  // Flag 정보
                                    );
	TopStudent->SetName(TEXT("이득우"));
	TopStudent->SetOrder(36);

	// Sub Object를 10개 생성해 보자
	const int32 NumofSubs = 10;
	for (int32 ix = 1; ix <= NumofSubs; ++ix)
	{
		FString SubObjectName = FString::Printf(TEXT("Student%d"), ix);
		UStudent* SubStudent = NewObject<UStudent>
            (
            TopStudent, 
            UStudent::StaticClass(), 
            *SubObjectName, 
            ObjectFlag
            );
		SubStudent->SetName(FString::Printf(TEXT("학생%d"), ix));
		SubStudent->SetOrder(ix);
	}

	// 패키지 저장
	const FString PackageFileName = FPackageName::LongPackageNameToFilename(PackageName, FPackageName::GetAssetPackageExtension());
	FSavePackageArgs SaveArgs;
	SaveArgs.TopLevelFlags = ObjectFlag;

	if (UPackage::SavePackage(StudentPackage, nullptr, *PackageFileName, SaveArgs))
	{
		UE_LOG(LogTemp, Log, TEXT("패키지가 성공적으로 저장되었습니다."));
	}

}
```

```cpp
void UMyGameInstance::LoadStudentPackage() const
{
	UPackage* StudentPackage = ::LoadPackage(nullptr, *PackageName, LOAD_None);
	if (nullptr == StudentPackage)
	{
		UE_LOG(LogTemp, Warning, TEXT("패키지를 찾을 수 없습니다."));
		return;
	}

	StudentPackage->FullyLoad();

	UStudent* TopStudent = FindObject<UStudent>(StudentPackage, *AssetName);
	PrintStudentInfo(TopStudent, TEXT("FindObject Asset"));
}
```

---

## Unreal의 애셋 정보의 저장과 로딩 전략

* 애셋의 사용시 매번 패키지를 불러 할당하는 작업은 부하가 크다
	* 애셋의 로딩 대신 패키지와 오브젝트를 지정한 문자열을 대체해 사용한다
	* 프로젝트 내에 오브젝트 경로 값은 유일함을 UE에서 보장한다
* 애셋의 로딩전략
	* 프로젝트에서 반드시 필요한 애셋 - 생성자에서 미리 로딩 후 사용
	* 동기/비동기 적으로 필요한 애셋 - 동기/비동기 적으로 생성 ...

---

* 패키지 이름과 애셋 이름을 한 데 묶은 문자열

```
{애셋클래스정보}'{패키지이름}.{애셋이름}'

혹은

{패키지이름}.{애셋이름}
```

```cpp
void UMyGameInstance::LoadStudentObject() const
{
	const FString TopSoftObjectPath = FString::Printf(TEXT("%s.%s"), *PackageName, *AssetName);

	UStudent* TopStudent = LoadObject<UStudent>(nullptr, *TopSoftObjectPath);
	PrintStudentInfo(TopStudent, TEXT("LoadObject Asset"));
}
```

* 생성자에서 애셋을 찾는 방법은 조금 다르다

```cpp
UMyGameInstance::UMyGameInstance()
{
	const FString TopSoftObjectPath = FString::Printf(TEXT("%s.%s"), *PackageName, *AssetName);
	static ConstructorHelpers::FObjectFinder<UStudent> UASSET_TopStudent(*TopSoftObjectPath);
	if (UASSET_TopStudent.Succeeded())
	{
		PrintStudentInfo(UASSET_TopStudent.Object, TEXT("Constructor"));
	}
}
```

* 비동기 로딩도 가능하다

```cpp
UCLASS()
class UNREALSERIALIZATION_API UMyGameInstance : public UGameInstance
{
    // ...

    // FStreamableManager선언
	FStreamableManager StreamableManager;
	TSharedPtr<FStreamableHandle> Handle;
};
```

```cpp
const FString TopSoftObjectPath = FString::Printf(TEXT("%s.%s"), *PackageName, *AssetName);
Handle = StreamableManager.RequestAsyncLoad(TopSoftObjectPath,
    [&]()
    {
        if (Handle.IsValid() && Handle->HasLoadCompleted())
        {
            UStudent* TopStudent = Cast<UStudent>(Handle->GetLoadedAsset());
            if (TopStudent)
            {
                PrintStudentInfo(TopStudent, TEXT("AsyncLoad"));

                Handle->ReleaseHandle();
                Handle.Reset();
            }
        }
    }
);
```