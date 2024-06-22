---
layout: default
title: "11. (Lyra) Shooter Core Plug-in"
parent: "(Example6 - Lyra 분석)"
grand_parent: "(Unreal C++ 🚀)"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## 시작

* Lyra의 Shooter Core Plug-in을 분석해 보자

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_6/ucpp6-11-1.png"/>
</p>

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_6/ucpp6-11-3.png"/>
</p>

---

## 시작은 역시 ShooterCore Data Asset

* Data Asset 이란?
    * 새로운 게임을 정의한다 생각하자.
    * 모든 세팅을 Project Settings에서 할 수 없으니 Data Asset에 나눈다라고 생각하면 편하다
* 가장 베이스가 되는 Data Asset은 ShooterCore Data Asset을 검색해 보면 된다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_6/ucpp6-11-2.png"/>
</p>

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_6/ucpp6-11-4.png"/>
</p>

* 뭔가 게임과 관련된 세팅이 잔뜩이다.
  * 우선 이걸 이해하기 위해선 [여기 🌟](https://miro.com/app/board/uXjVPvPBawA=/)를 많이 참조하자

```cpp
UCLASS(BlueprintType, Const)
class ULyraExperienceDefinition : public UPrimaryDataAsset
```

* `ULyraExperienceDefinition`에서 상속해 ShooterCore.data를 사용하게 된다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_6/ucpp6-11-6.png"/>
</p>

* 순서를 좀 정리해 보자면 ...
  * 우선, ShooterGym으로 들어간다고 가정
  * LyraExperienceDefinition의 B_BasicShooterTest를 통하여 ShooterCore.DataAsset 로드(Enabled)
  * DA_ShooterGame_ShooterGym 에 등록된 데이터를 로드

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_6/ucpp6-11-7.png"/>
</p>

---

## Example. Weapon

* ID_Pistol을 먼저 살펴보자

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_6/ucpp6-11-5.png"/>
</p>

> 여기서 부터는 직접 분석해 보는게 좋음.
