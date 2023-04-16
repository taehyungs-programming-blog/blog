---
layout: default
title: "01. 환경설정"
parent: "(Unreal C++)"
grand_parent: "(Unreal 🚀)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## 환경

* Visual Studio Community 2022
    * C++을 이용한 게임개발 설치할 것
    * [Visual Commander](https://marketplace.visualstudio.com/items?itemName=SergeyVlasov.VisualCommander)도 다운받아 두자
    * [ue4-vs-extensions](https://github.com/hackalyze/ue4-vs-extensions)도 다운받아 두자
    * Visual Commander에서 us4-vs-extensions을 Import하자
        * 다운받은 `ue4_smarter_macro_indenting_vs2017-2019.vcmd`를 Import하면 됨.
* Unreal 5.1.1
    * 이후에 심볼도 사용되니 심볼까지 설치하는것 추천

* (추가) [Jetbrains의 Rider](https://www.jetbrains.com/ko-kr/rider/)가 최근에 많이 사용된다고 하니 참고(빠르다고 한다.)

---

## 프로젝트 생성

* [Get Code 🌎](https://github.com/Arthur880708/Unreal_Cpp_basic/tree/main/1.HelloUnreal)

* Blank -> C++ -> 3D

---

## Hello World 출력하기

* 툴 -> C++ 클래스 추가 -> GameInstance 추가

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/basic_cpp/basic_cpp_1-1.png"/>
</p>

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/basic_cpp/basic_cpp_1-2.png"/>
</p>

* 기본으로 로딩되는 Map이 무겁기에 간단한 Map을 만들어서 로딩시간을 줄이자.
* UE5 에디터 -> 편집 -> 프로젝트 세팅 -> 맵 & 모드 -> Default Maps
    * 모두 Clear로 설정
    * UE5 에디터를 재시작해야한다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/basic_cpp/basic_cpp_1-7.png"/>
</p>

### Unreal Compile 주의사항!

* 헤더 파일에 변경이 발생 -> UE5에디터를 끄고 **비주얼 스튜디오에서 컴파일**
* 소스 파일에 변경이 발생 -> UE5에서 **라이브 코딩으로 컴파일(Ctrl + Alt + F11)**
* **주의** - 비주얼 스튜디오에서 수동으로 클래스를 추가하질 말 것

* 말로만 설명하면 어려우니 예시로 보자.

### UE5 에디터가 켜진상태로 비쥬얼스튜디오 빌드를 해보면?

* 아래와 같이 Build Fail

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/basic_cpp/basic_cpp_1-3.png"/>
</p>

### UE5 에디터를 끄고 비쥬얼스튜디오 빌드 진행

* 빌드가 정상적으로 된다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/basic_cpp/basic_cpp_1-4.png"/>
</p>

### 그럼! 헤더파일을 변경해 보자.

```cpp
UCLASS()
class UNREAL5_EXAMPLE_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
    // Init을 상속받으려 함.
	virtual void Init() override;
};
```

```cpp
void UMyGameInstance::Init()
{
	Super::Init();

	UE_LOG(LogTemp, Log, TEXT("%s"), TEXT("Hello Unreal"));
}
```

* Ctrl + F5를 하면 에디터가 자동실행 된다.
* 기본로그가 많기에 검색해야 함!

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/basic_cpp/basic_cpp_1-5.png"/>
</p>

* UE5 에디터 -> 편집 -> 프로젝트 세팅 -> 맵 & 모드 -> 게임 인스턴스 클래스를 내가 만든 게임 인스턴스클래스로 변경

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/basic_cpp/basic_cpp_1-6.png"/>
</p>