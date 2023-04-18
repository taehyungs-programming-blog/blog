---
layout: default
title: "11. 직렬화"
parent: "(Unreal C++)"
grand_parent: "(Unreal 🚀)"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌎](https://github.com/Arthur880708/Unreal_Cpp_basic/tree/main/10.Serialization)

## C++ struct의 직렬화

```cpp
// 아래와 같은 struct가 있다고 가정하자
struct FStudentData
{
	FStudentData() {}
	FStudentData(int32 InOrder, const FString& InName) : Order(InOrder), Name(InName) {}

	friend FArchive& operator<<(FArchive& Ar, FStudentData& InStudentData)
	{
		Ar << InStudentData.Order;
		Ar << InStudentData.Name;
		return Ar;
	}

	int32 Order = -1;
	FString Name = TEXT("홍길동");
};
```

```cpp
void UMyGameInstance::Init()
{
	Super::Init();

	FStudentData RawDataSrc(16, TEXT("이득우"));

	const FString SavedDir = FPaths::Combine(FPlatformMisc::ProjectDir(), TEXT("Saved"));
	UE_LOG(LogTemp, Log, TEXT("저장할 파일 폴더 : %s"), *SavedDir);

	{
        // WRITE
		const FString RawDataFileName(TEXT("RawData.bin"));
		FString RawDataAbosolutePath = FPaths::Combine(*SavedDir, *RawDataFileName);
		UE_LOG(LogTemp, Log, TEXT("저장할 파일 전체 경로 : %s"), *RawDataAbosolutePath);
		FPaths::MakeStandardFilename(RawDataAbosolutePath);
		UE_LOG(LogTemp, Log, TEXT("변경할 파일 전체 경로 : %s"), *RawDataAbosolutePath);

		FArchive* RawFileWriterAr = IFileManager::Get().CreateFileWriter(*RawDataAbosolutePath);
		if (nullptr != RawFileWriterAr)
		{
			*RawFileWriterAr << RawDataSrc;
			RawFileWriterAr->Close();
			delete RawFileWriterAr;
			RawFileWriterAr = nullptr;
		}

        // READ
		FStudentData RawDataDest;
		FArchive* RawFileReaderAr = IFileManager::Get().CreateFileReader(*RawDataAbosolutePath);
		if (nullptr != RawFileReaderAr)
		{
			*RawFileReaderAr << RawDataDest;
			RawFileReaderAr->Close();
			delete RawFileReaderAr;
			RawFileReaderAr = nullptr;

			UE_LOG(LogTemp, Log, TEXT("[RawData] 이름 %s 순번 %d"), *RawDataDest.Name, RawDataDest.Order);
		}
	}

    // ...
```

---

## Unreal Object 직렬화

```cpp
// UPROPERTY() TObjectPtr<class UStudent> StudentSrc;
StudentSrc = NewObject<UStudent>();
StudentSrc->SetName(TEXT("이득우"));
StudentSrc->SetOrder(59);

{
    const FString ObjectDataFileName(TEXT("ObjectData.bin"));
    FString ObjectDataAbsolutePath = FPaths::Combine(*SavedDir, *ObjectDataFileName);
    FPaths::MakeStandardFilename(ObjectDataAbsolutePath);

    TArray<uint8> BufferArray;
    FMemoryWriter MemoryWriterAr(BufferArray);
    StudentSrc->Serialize(MemoryWriterAr);
/*
void UStudent::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);

	Ar << Order;
	Ar << Name;
}
*/
    
    if (TUniquePtr<FArchive> FileWriterAr = TUniquePtr<FArchive>(IFileManager::Get().CreateFileWriter(*ObjectDataAbsolutePath)))
    {
        *FileWriterAr << BufferArray;
        FileWriterAr->Close();
    }

    TArray<uint8> BufferArrayFromFile;
    if (TUniquePtr<FArchive> FileReaderAr = TUniquePtr<FArchive>(IFileManager::Get().CreateFileReader(*ObjectDataAbsolutePath)))
    {
        *FileReaderAr << BufferArrayFromFile;
        FileReaderAr->Close();
    }

    FMemoryReader MemoryReaderAr(BufferArrayFromFile);
    UStudent* StudentDest = NewObject<UStudent>();
    StudentDest->Serialize(MemoryReaderAr);
    PrintStudentInfo(StudentDest, TEXT("ObjectData"));
}
```

---

## JSON 직렬화

* 우선 Json 관련 lib를 추가해 줘야한다.

```csharp
// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class UnrealSerialization : ModuleRules
{
	public UnrealSerialization(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        // "Json", "JsonUtilities"를 추가해 줘야한다.
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "Json", "JsonUtilities" });

		PrivateDependencyModuleNames.AddRange(new string[] { });

    // ...
```

```cpp	
{
    const FString JsonDataFileName(TEXT("StudentJsonData.txt"));
    FString JsonDataAbsolutePath = FPaths::Combine(*SavedDir, *JsonDataFileName);
    FPaths::MakeStandardFilename(JsonDataAbsolutePath);

    TSharedRef<FJsonObject> JsonObjectSrc = MakeShared<FJsonObject>();
    FJsonObjectConverter::UStructToJsonObject(StudentSrc->GetClass(), StudentSrc, JsonObjectSrc);

    FString JsonOutString;
    TSharedRef<TJsonWriter<TCHAR>> JsonWriterAr = TJsonWriterFactory<TCHAR>::Create(&JsonOutString);
    if (FJsonSerializer::Serialize(JsonObjectSrc, JsonWriterAr))
    {
        FFileHelper::SaveStringToFile(JsonOutString, *JsonDataAbsolutePath);
    }

    FString JsonInString;
    FFileHelper::LoadFileToString(JsonInString, *JsonDataAbsolutePath);

    TSharedRef<TJsonReader<TCHAR>> JsonReaderAr = TJsonReaderFactory<TCHAR>::Create(JsonInString);

    TSharedPtr<FJsonObject> JsonObjectDest;
    if (FJsonSerializer::Deserialize(JsonReaderAr, JsonObjectDest))
    {
        UStudent* JsonStudentDest = NewObject<UStudent>();
        if (FJsonObjectConverter::JsonObjectToUStruct(JsonObjectDest.ToSharedRef(), JsonStudentDest->GetClass(), JsonStudentDest))
        {
            PrintStudentInfo(JsonStudentDest, TEXT("JsonData"));
        }
    }
}
```