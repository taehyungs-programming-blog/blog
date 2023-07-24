---
layout: default
title: "01. 환경 및 프로젝트 세팅"
parent: "(Unreal C++ - Basic Example)"
grand_parent: "(Unreal 🚀)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## 환경, 프로젝트 세팅

* Unreal 5.0.X
* Visual Studio 2019
* 마켓플레이서 준비
    * Unreal Learning Kit: Games 준비
    * Stylized Character Kit: Casual01 준비
* 프로젝트 세팅
    * Unreal Learning Kit: Games에서 프로젝트 생성
    * 기본 시작된 Map을 MainMap으로 저장 후
    * Project Settings -> Maps & Modes로 들어가 기본 Map으로 설정한다
* Visual Studio 세팅
    * 2022를 사용하는 것을 추천(2019도 상관은 없음)
    * C++를 사용한 게임 개발을 설치하며 Unreal Engine 설치 관리자가 포함되어야 함을 주의

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/cpp-basic-example-1/cpp-basic-example-1-1.png"/>
</p>

* Visual Studio Code Extension
    * C/C++
    * Unreal Engine 4 Snippets

---

## 캐릭터 커스텀

* Stylized Character Kit: Casual01에서 ThirdPersonCharacter Mesh를 하나가져온다
* 가져온 ThirdPersonCharacter Mesh를 BP_ThirdPersonCharacter라는 이름의 Blueprint Class로 저장한다
* Blueprint Class 내부
    * Auto Possess Player를 Player0로 설정한다
* 상속한 ThirdPersonChacter Event Graph에 Warning이 두 개 나있을건데 아래와 같이 해결한다

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/cpp-basic-example-1/cpp-basic-example-1-2.png"/>
</p>

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/cpp-basic-example-1/cpp-basic-example-1-3.png"/>
</p>

---

## C++ 컴파일

* Editor 수정
    * Edit -> Editor Perferences -> General -> Source Code -> Visual Studio Code
* Tools -> New C++ Class를 통해 Actor하나를 생성해보자
* 생성 후 Tools -> Open Visual Studio Code 실행
    * Unreal Eidtor 없이 Visual Studio Code만 열고 싶다면 .code-workspace파일을 우클릭 후 Visual Studio Code로 열기를 하면 된다.
* 컴파일
    * Terminal -> Run Build Task...(Ctrl+Shift+B) 클릭
    * 다양한 빌드옵션이 있는데 Win64 Development Build를 선택해야한다
