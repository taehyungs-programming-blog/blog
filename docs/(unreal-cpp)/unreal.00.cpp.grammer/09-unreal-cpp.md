---
layout: default
title: "09. Unreal Container"
parent: "(문법)"
grand_parent: "(Unreal C++ 🚀)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌎](https://github.com/Arthur880708/Unreal_Cpp_basic/tree/main/8.Container)

* TArray, TSet을 이해하자!

* STL로 생각하자면 ...
    * `std::vector` -> `TArray`
    * `std::set` -> `TSet`
    * `std::map` -> `TMap`

* 정리하자면 ...
    * `TArray` - 접근 속도 O(1) / 검색, 삽입, 삭제 속도 O(N) / 빈틈 없는 메모리, 높은 접근성능, 높은 순회성능
    * `TSet` - 접근, 검색, 삽입, 삭제 속도 O(1) / 빠른 중복 감지
    * `TMap` - 접근, 검색, 삽입, 삭제 속도 O(1) / 중복 불허, 키 밸류 관리

* UStruct
    * 데이터 저장/전송에 특화된 가벼운 객체
    * `USTRUCT()` 통해 선언
    * `UFUNCTION` 은 쓸수 없음

---

## TArray

* `std::array`와 다름! `std::vector`와 같이 가변 메모리임.

```cpp
USTRUCT()
struct FStudentData
{
	GENERATED_BODY()
	FStudentData()
	{
		Name = TEXT("홍길동");
		Order = -1;
	}

	FStudentData(FString InName, int32 InOrder) : Name(InName), Order(InOrder) {}

	bool operator==(const FStudentData& InOther) const
	{
		return Order == InOther.Order;
	}

	friend FORCEINLINE uint32 GetTypeHash(const FStudentData& InStudentData)
	{
		return GetTypeHash(InStudentData.Order);
	}

	UPROPERTY()
	FString Name;

	UPROPERTY()
	int32 Order;
};

UCLASS()
class UNREALCONTAINER_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:

	virtual void Init() override;
private:
	TArray<FStudentData> StudentsData;

	UPROPERTY()
	TArray<TObjectPtr<class UStudent>> Students;

	TMap<int32, FString> StudentsMap;
};
```

```cpp
void UMyGameInstance::Init()
{
	Super::Init();

	const int32 ArrayNum = 10;
	TArray<int32> Int32Array;

    // Add
	for (int32 ix = 1; ix <= ArrayNum; ++ix)
	{
		Int32Array.Add(ix);
	}

    // Remove
	Int32Array.RemoveAll(
		[](int32 Val)
		{
			return Val % 2 == 0;
		}
	);

	Int32Array += {2, 4, 6, 8, 10};

	TArray<int32> Int32ArrayCompare;
	int32 CArray[] = { 1, 3, 5, 7, 9, 2, 4, 6, 8 , 10 };
        // 초기화
	Int32ArrayCompare.AddUninitialized(ArrayNum);
    // Memcpy도 동작한다
	FMemory::Memcpy(Int32ArrayCompare.GetData(), CArray, sizeof(int32) * ArrayNum);

	ensure(Int32Array == Int32ArrayCompare);

	int32 Sum = 0;
    // iterator 동작
	for (const int32& Int32Elem : Int32Array)
	{
		Sum += Int32Elem;
	}

	ensure(Sum == 55);

    // Sum 함수
	int32 SumByAlgo = Algo::Accumulate(Int32Array, 0);
	ensure(Sum == SumByAlgo);

	// ...

    // object를 넣을수 있음
	const int32 StudentNum = 300;
	for (int32 ix = 1; ix <= StudentNum; ++ix)
	{
		StudentsData.Emplace(FStudentData(MakeRandomName(), ix));
	}

	TArray<FString> AllStudentsNames;
	Algo::Transform(StudentsData, AllStudentsNames,
		[](const FStudentData& Val)
		{
			return Val.Name;
		}
	);

	UE_LOG(LogTemp, Log, TEXT("모든 학생 이름의 수 : %d"), AllStudentsNames.Num());

    // ...
}
```

---

## TSet

* 이진트리로 구성되어있어 자동으로 정렬이 된다.
* 빠른 검색이 가능하지만 요소의 추가, 삭제시 역시 자동정렬이 된다.

```cpp
// add
TSet<int32> Int32Set;
for (int32 ix = 1; ix <= ArrayNum; ++ix)
{
    Int32Set.Add(ix);
}

// remove
Int32Set.Remove(2);
Int32Set.Remove(4);
Int32Set.Remove(6);
Int32Set.Remove(8);
Int32Set.Remove(10);
Int32Set.Add(2);
Int32Set.Add(4);
Int32Set.Add(6);
Int32Set.Add(8);
Int32Set.Add(10);

TSet<FString> AllUniqueNames;
Algo::Transform(StudentsData, AllUniqueNames,
    [](const FStudentData& Val)
    {
        return Val.Name;
    }
);

UE_LOG(LogTemp, Log, TEXT("중복 없는 학생 이름의 수 : %d"), AllUniqueNames.Num());
```

---

## TMap

```cpp
Algo::Transform(StudentsData, StudentsMap,
    [](const FStudentData& Val)
    {
        return TPair<int32, FString>(Val.Order, Val.Name);
    }
);

UE_LOG(LogTemp, Log, TEXT("순번에 따른 학생 맵의 레코드 수 : %d"), StudentsMap.Num());

TMap<FString, int32> StudentsMapByUniqueName;

Algo::Transform(StudentsData, StudentsMapByUniqueName,
    [](const FStudentData& Val)
    {
        return TPair<FString, int32>(Val.Name, Val.Order);
    }
);

UE_LOG(LogTemp, Log, TEXT("이름에 따른 학생 맵의 레코드 수 : %d"), StudentsMapByUniqueName.Num());


TMultiMap<FString, int32> StudentMapByName;
Algo::Transform(StudentsData, StudentMapByName,
    [](const FStudentData& Val)
    {
        return TPair<FString, int32>(Val.Name, Val.Order);
    }
);

UE_LOG(LogTemp, Log, TEXT("이름에 따른 학생 멀티맵의 레코드 수 : %d"), StudentMapByName.Num());

const FString TargetName(TEXT("이혜은"));
TArray<int32> AllOrders;
StudentMapByName.MultiFind(TargetName, AllOrders);

UE_LOG(LogTemp, Log, TEXT("이름이 %s인 학생 수 : %d"), *TargetName, AllOrders.Num());

TSet<FStudentData> StudentsSet;
for (int32 ix = 1; ix <= StudentNum; ++ix)
{
    StudentsSet.Emplace(FStudentData(MakeRandomName(), ix));
}
```