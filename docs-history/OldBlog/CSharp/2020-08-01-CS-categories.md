---
title: "C#"
permalink: cs/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-08-01 00:00:00 -0000
last_modified_at: 2020-08-01 00:00:00 -0000
header:
  teaser: /file/image/cs-page-teaser.gif
sidebar:
  title: "C#" 
  nav: cs
tag:
  - C#
category:
  - category
excerpt: "C#와 관련된 지식을 정리한 페이지 입니다."
header:
  teaser: /file/image/cs-page-teaser.gif
  overlay_image: /file/image/main-page.jpg
  overlay_filter: 0.1 # rgba(255, 0, 0, 0.5)
  caption: "Photo credit: [**EBS**](https://ebs.co.kr)"
  actions:
    - label: "C# HomePage"
      url: "https://docs.microsoft.com/ko-kr/dotnet/csharp/"
---

## C# 기초강좌

* [C# Basic - 1 : 실습환경](/cs-basic-1/)
* [C# Basic - 2: .NET Framework 란?](/cs-basic-2/)
* [C# Basic - 3: 표준입출력](/cs-basic-3/)
* [C# Basic - 4: 타입, 변수, 리터럴](/cs-basic-4/)
* [C# Basic - 5: 클래스 기본 문법](/cs-basic-5/)

* [C# Basic - 6: System.Object](/cs-basic-6/)
* [C# Basic - 7: Value type & Reference Type](/cs-basic-7/)
* [C# Basic - 8: null & casting](/cs-basic-8/)
* [C# Basic - 9: Boxing / Unboxing](/cs-basic-9/)

* [C# Basic - 10: Array](/cs-basic-10/)
* [C# Basic - 11: 인자 전달 방식](/cs-basic-11/)
* [C# Basic - 12: 메소드 특징](/cs-basic-12/)
* [C# Basic - 13: Property and Indexer](/cs-basic-13/)
* [C# Basic - 14: Index and Range](/cs-basic-14/)
* [C# Basic - 15: Generic](/cs-basic-15/)
* [C# Basic - 16: Delegate](/cs-basic-16/)
* [C# Basic - 17: Delegate2](/cs-basic-17/)
* [C# Basic - 18: Exception Handling](/cs-basic-18/)
* [C# Basic - 19: try ~ finally](/cs-basic-19/)

* [C# Basic - 20: Collection](/cs-basic-20/)
* [C# Basic - 21: Coroutine](/cs-basic-21/)
* [C# Basic - 22: LINQ](/cs-basic-22/)
* [C# Basic - 23: Threading](/cs-basic-23/)
* [C# Basic - 24: Garbage Collection & Disposal Pattern](/cs-basic-24/)

---

## WPF

### Intro to WPF

* [WPF - 1: 프로젝트생성](/cs-wpf-01/)

![](/file/image/cs-wpf-01-02.png)

### Calculator App

* [WPF - 2: XAML이란](/cs-wpf-02/)

![](/file/image/cs-wpf-2-4.png)

* [WPF - 3: Improving the UI](/cs-wpf-03/)

![](/file/image/cs-wpf-3-2.png)

* [WPF - 4: SQLite](/cs-wpf-04/)

* [WPF - 5: ListView](/cs-wpf-05/)

![](/file/image/cs-wpf-5-4.png)

* [WPF - 6: Custom User Controls](/cs-wpf-06/)
* [WPF - 7: REST and AI](/cs-wpf-07/)
* [WPF - 8: MVVM - The Weather App](/cs-wpf-08/)

---

## Unity

> 실습에 사용한 Unity Version은 **19.2.x** 사용

* [기본 세팅](/unity/00/)

* [바닥, 움직이는 구, 간단한 스크립트 만들기](/unity/01/)
* [키보드 입력을 움직이는 구에 주기](/unity/02/)
* [Time.deltaTime이용 구 회전시키기](/unity/03/)
* [충돌 감지 함수, 모든 오브젝트 효과적용하기](/unity/04/)
* [GameManager 생성 및 배열적용](/unity/05/)

* [Object의 상대적 좌표, 스페이스바 누를시 점프](/unity/06/)
* [쿼터니언이란?, Object를 회전시키기](/unity/07/)
* [Object 복제, Object 생성위치 지정](/unity/08/)
* [전역(static)변수 사용하기](/unity/09/)
* [Asset Store이용](/unity/10/)

* [Coroutine 유니티 쓰레드 사용하기](/unity/11/)
* [Unity Event](/unity/12/)

## C#

* [오버로드](/unity/csharp/overload/)
* [List](/unity/csharp/list/)
* [Singleton Pattern](/unity/csharp/singleton/)
* [상속](/unity/csharp/inheritance/)
* [interface](/unity/csharp/interface/)

* [추상(abstract) 클래스](/unity/csharp/abstract/)
* [프로퍼티 (getter, setter)](/unity/csharp/property/)
* [delegate](/unity/csharp/delegate/)
* [event](/unity/csharp/event/)
* [액션, 람다 함수](/unity/csharp/action-lambda/)

* [제네릭(템플릿)]()

```csharp
public void Print<T>(T inputMessage)
{
  Debug.Log(inputMessage)
}

public class Container<T>
{
  T messages[]
}
```

* 기본적 문법

```csharp
// 주석

/* 
* 주석
*/

// 콘솔 로그 출력
int num = 0;
Debug.Log("로그" + num);

// 함수선언
float GetRadius(float size)
{
  // ...
}

// 배열 선언
int[] scores = new int[10];

// 상속
public class Animal
{
  public string name;
  string sound;
  float weight;
}

Animal jack = new Animal();
```