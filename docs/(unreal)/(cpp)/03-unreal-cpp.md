---
layout: default
title: "03. Unreal 기본타입과 문자열"
parent: "(Unreal C++)"
grand_parent: "(Unreal 🚀)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌎](https://github.com/Arthur880708/Unreal_Cpp_basic/tree/main/2.UnrealString)

## 이론

* Unreal 이 별도의 타입규약을 갖는이유?
    * 최신 C++ 규약에서는 int는 최소 32bit를 보장한다.
    * 플랫폼에 따라 64bit로 해석되기도 하는데 ... 게임에선 이렇게 돼어선 안된다.
    * 데이터 타입의 애매 모호함은 게임 개발시 문제를 유발할 수 있다.
* 문자열은 왜?
    * 문자열의 처리 종류가 너무 다양하다(ANSI, ASCII, CP949 ...)
    * Unreal의 경우 UTF-16이다.

---

## 주의할 점!

* 코드의 저장을 UTF-8로 해줘야 한다.
* 아니면 코드에서 한글을 하드코딩할시 읽어오질 못한다.

* 파일 -> 다른 이름으로 저장 -> 저장 옵션 -> 인코딩 하여 저장

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/basic_cpp/basic_cpp_3-1.png"/>
</p>

---

## 예제를 살펴보자.

### char

```cpp
// char 사용하기
TCHAR LogCharArray[] = TEXT("Hello Unreal");
UE_LOG(LogTemp, Log, LogCharArray);
```

### string

```cpp
// string 사용하기
FString LogCharString = LogCharArray;
UE_LOG(LogTemp, Log, TEXT("%s"), *LogCharString);
```

### char*

```cpp
// char* 사용하기
const TCHAR* LongCharPtr = *LogCharString;
TCHAR* LogCharDataPtr = LogCharString.GetCharArray().GetData();
```

### char 배열

```cpp
// char 배열 사용하기
TCHAR LogCharArrayWithSize[100];
FCString::Strcpy(LogCharArrayWithSize, LogCharString.Len(), *LogCharString);
```

```cpp
// string 함수 써보기
if (LogCharString.Contains(TEXT("unreal"), ESearchCase::IgnoreCase))
{
    int32 Index = LogCharString.Find(TEXT("unreal"), ESearchCase::IgnoreCase);
    FString EndString = LogCharString.Mid(Index);
    UE_LOG(LogTemp, Log, TEXT("Find Test: %s"), *EndString);
}

FString Left, Right;
if (LogCharString.Split(TEXT(" "), &Left, &Right))
{
    UE_LOG(LogTemp, Log, TEXT("Split Test: %s 와 %s"), *Left, *Right);
}
```

```cpp
// 데이터 타입 to string
int32 IntValue = 32;
float FloatValue = 3.141592;

FString FloatIntString = FString::Printf(TEXT("Int:%d Float:%f"), IntValue, FloatValue);
FString FloatString = FString::SanitizeFloat(FloatValue);
FString IntString = FString::FromInt(IntValue);

UE_LOG(LogTemp, Log, TEXT("%s"), *FloatIntString);
UE_LOG(LogTemp, Log, TEXT("Int:%s Float:%s"), *IntString, *FloatString);

int32 IntValueFromString = FCString::Atoi(*IntString);
float FloatValueFromString = FCString::Atof(*FloatString);
FString FloatIntString2 = FString::Printf(TEXT("Int:%d Float:%f"), IntValueFromString, FloatValueFromString);
UE_LOG(LogTemp, Log, TEXT("%s"), *FloatIntString2);
```

### FName

* FName과 FText
* FName
    * 애셋 관리를 위해 사용되는 문자열(애셋을 검색 등)
    * 대소문자 구분이 없다.
    * 한 번 선언되면 바뀌지 않는다.
    * 가볍고 빠르다.
    * 문자를 표현용도가 아닌 애셋 키를 저장하는 용도
* FText
    * 일종의 키로 사용함
    * 이후에 다시 설명 예정

```cpp
// 비교연산
FName key1(TEXT("PELVIS" /*value*/));
FName key2(TEXT("pelvis"));

    // 두 키값이 같다(대소문자를 구분하지 않음)
UE_LOG(LogTemp, Log, TEXT("FName 비교 결과 : %s"), key1 == key2 ? TEXT("같음") : TEXT("다름"));

    // (TIPS) 빈번하게 호출되는 key의 경우 
    // static으로 선언하여 사용하면 더 빠르다.
for (int i = 0; i < 10000; ++i)
{
    FName SearchInNamePool = FName(TEXT("pelvis"));
    const static FName StaticOnlyOnce(TEXT("pelvis"));
}
```

* FName은 FNamePool에 저장되어있는데, 이를 통해 검색속도가 빠르다.
* FNamePool은 Key와 Value로 구성되어있다.
