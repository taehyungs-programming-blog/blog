---
layout: default
title: "07-01. (보충) Unreal의 New! 할당"
parent: "(문법)"
grand_parent: "(Unreal C++ 🚀)"
great_grand_parent: "Legacy Archive"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* 우린 Unreal Object를 생성하기 위해 `NewObject<UClass>()`를 써야함을 안다
* 그런데 만약 아래와 같이 간단한 클래스라면?

```cpp
class MyData {
public:
    int32 data1;
    int32 data2;
};

// 이렇게 써도 좋고
TArray<MyData> MyDataArray;
```

* Editor 접근이 필요하다면

```cpp
USTRUCT(BlueprintType)
struct FMyData {
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadWrite, Category="Data")
    int32 data1;

    UPROPERTY(BlueprintReadWrite, Category="Data")
    int32 data2;
};

// Unreal Object 내에서
UPROPERTY(BlueprintReadWrite, Category="Data")
TArray<FMyData> MyDataArray;
```

```cpp
UCLASS(BlueprintType)
class UMyData : public UObject {
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadWrite, Category="Data")
    int32 data1;

    UPROPERTY(BlueprintReadWrite, Category="Data")
    int32 data2;
};
```