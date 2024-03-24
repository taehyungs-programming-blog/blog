---
layout: default
title: "01. Engine Build + Open Source 분석하는 법"
parent: "(Engine Source Code 분석 1)"
grand_parent: "(Unreal C++ 🚀)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## Build

* [Get Repo 🌟](https://github.com/EpicGames/UnrealEngine)
    * Epic 개발자 권한이 필요한데 그 부분은 생략!

1. `Setup.bat` 실행
2. `GenerateProjectFile.bat` 실행

---

## Open Source 분석하는 법

* 오픈소스는 코드양이 상당히 방대하다
* 한 부분을 잡고 분석하는게 좋고, 코드를 Clone하여 Note하며 분석한다

```cpp
// Example

/*
Note: 23.03.24 - ActionClass
* 이 코드는 ~~~~ 를 하기 위한 코드임
* Q) 여기서 ~~~~ 의 목적은 뭐지?
*/
```

* **Unreal기준 코드에 남겨져 있는 주석은 무조건 읽어봐야한다**

---
