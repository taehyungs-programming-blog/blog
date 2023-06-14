---
title: "Unity"
permalink: unity/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-06-02 00:00:00 -0000
last_modified_at: 2020-06-02 00:00:00 -0000
header:
  teaser: /file/image/unity-page-teaser.gif
sidebar:
  title: "unity" 
  nav: unity
tag:
  - Unity
category:
  - category
excerpt: "Unity 및 C#과 관련된 지식을 정리한 페이지 입니다."
header:
  overlay_image: /file/image/main-page.jpg
  overlay_filter: 0.1 # rgba(255, 0, 0, 0.5)
  caption: "Photo credit: [**EBS**](https://ebs.co.kr)"
  actions:
    - label: "Unity HomePage"
      url: "https://unity3d.com//"
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