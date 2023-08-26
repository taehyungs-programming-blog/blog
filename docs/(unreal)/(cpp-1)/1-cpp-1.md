---
layout: default
title: "01. 캐릭터 세팅"
parent: "(Unreal C++ Part2 - 게임만들어 보기)"
grand_parent: "(Unreal 🚀)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* 다운 받아야할 패키지가 많아서 될 수 있으면 코드를 Clone 받자

* [Get Code 🌎](https://github.com/Arthur880708/Unreal_RPG_Tutorial_1)

* 환경
    * UE 5.0.3
    * Visual Studio 2022

---

## 패키지 Import시 주의사항, 애니메이션 Import 하기

* EnbaleRootMotion과 Force Root Lock을 Enable시켜놔야 한다

---

## 캐릭터에 사용될 Skeletal Mesh 준비

* 아래와 같이 Skeletal Meshx에 Material만 수정해 여러개를 만들어 두자

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_1/1-cpp-1-1.png"/>
</p>

---

## 모든 애니메이션을 공통으로 쓰기위해 IK Rig을 만들자

* 혹시나 해서, prlvis는 항상 Root로 둬야한다

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_1/1-cpp-1-2.png"/>
</p>

* 이렇게 모든 Skeletal Mesh의 IK Rig을 생성 후
* IK Retargetor를 만들어 애니메이션이 잘 동작하나 확인한다

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_1/1-cpp-1-3.png"/>
</p>

* 애니메이션 우 클릭 후 Retarget Animation Assets -> Retarget animation Asset

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_1/1-cpp-1-4.png"/>
</p>

* 여기까지하면 애니메이션을 쓸 준빈 완료

---

