---
title: "C++ Design Pattern"
permalink: cpp/desing-pattern/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-03-15 00:00:00 -0000
last_modified_at: 2020-03-15 00:00:00 -0000
sidebar:
  title: "C++"
  nav: cpp
---

## Section 1. : Warming Up

* [protected constructor](https://8bitscoding.github.io/cpp-dp-protected-constructor/)
* [upcasting](https://8bitscoding.github.io/cpp-dp-upcasting/)
* [abstract class, interface & coupling](https://8bitscoding.github.io/cpp-dp-coupling/)
* [예제](https://8bitscoding.github.io/cpp-dp-example/)

## Section 2. : 공통성과 가변성의 분리

* [edit control 만들기](https://8bitscoding.github.io/cpp-dp-s2-1/)
* [변하는 것을 다른 클래스로](https://8bitscoding.github.io/cpp-dp-s2-3/) : template Pattern, Strategy Pattern 비교
* [Policy Base Design](https://8bitscoding.github.io/cpp-dp-s2-4/) : Strategy Pattern, Policy Base Pattern 비교
* [Application Framework](https://8bitscoding.github.io/cpp-dp-s2-5/) : template pattern 사용 대표 예시
* [함수와 정책](https://8bitscoding.github.io/cpp-dp-s2-6/) : Strategy Pattern을 함수에 적용
* [state pattern](https://8bitscoding.github.io/cpp-dp-s2-7/) : 오브젝트의 함수(동작)만 수정하고 싶을때 사용하는 패턴 -> Strategy pattern과 동일

* 정리 : 변하지 않는 코드에서 변해야 하는 부분은 분리하자
    - 일반 함수에서 변한다면? -> 함수 인자로 분리(함수 포인터, 함수객체, 람다 표현식)
    - 멤버 함수에서 변한다면?
        - 가상 함수로 분리 -> template pattern
            - 실행시간에 교체할 수 없고, 변하는 코드를 재사용할 수 없다.
        - 상속기반의 패턴
        - 다른 클래스로 분리 -> Strategy, Policy Base Design
        - 인터페이스로 교체 -> Strategy
            - 실행시간 교체가능, 가상함수 기반(느리다.)
        - 템플릿 인자로 교체 -> Policy Base Design
            - 실행시간 교체불간으, 인라인 치환 가능(빠르다)

## Section 3. : 재귀적 포함

* [Composite Pattern](https://8bitscoding.github.io/cpp-dp-s3-1/) : Menu 구현을 통한 Composite Pattern 설명
* [Menu Event](https://8bitscoding.github.io/cpp-dp-s3-2/) : Menu Command 처리, 일반/멤버 함수 포인터 처리 ,function 템플릿 사용법
* [Decorator](https://8bitscoding.github.io/cpp-dp-s3-3/) : Decorator pattern 설명

## Section 4. 간접층의 원리

* [Adapter](https://8bitscoding.github.io/cpp-dp-s4-1/) : stack, reverse_iterator
* [Proxy](https://8bitscoding.github.io/cpp-dp-s4-2/)
* [Facade](https://8bitscoding.github.io/cpp-dp-s4-3/)
* [Bridge](https://8bitscoding.github.io/cpp-dp-s4-4/)

* 정리 : 해결하기 어려운 문제를 간접층을 도입해서 해결하는 방법을 설명한다.
    - 인터페이스 변경 : Adapter
    - 대행자 : Proxy
    - 서브시스템 복잡한 과정을 숨긴다 : Facade
    - Update를 독맂벅으로 수행 : Bridge

## Section 5. : 통보, 열거, 방문

* [Observer](https://8bitscoding.github.io/cpp-dp-s5-1/)
* [Container](https://8bitscoding.github.io/cpp-dp-s5-2/)
* [Iterator](https://8bitscoding.github.io/cpp-dp-s5-3/)
* [Visitor](https://8bitscoding.github.io/cpp-dp-s5-4/) : 오브젝트에 함수를 추가한 효과를 내고 싶다

## Section 6. : 객체를 생성하는 방법

* [객체를 생성하는 방법](https://8bitscoding.github.io/cpp-dp-s6-1/)
* [Singleton](https://8bitscoding.github.io/cpp-dp-s6-2/)
* [Factory](https://8bitscoding.github.io/cpp-dp-s6-3/)
* [Abstract Factory](https://8bitscoding.github.io/cpp-dp-s6-4/)
* [Factory Method](https://8bitscoding.github.io/cpp-dp-s6-5/)
* [Builder](https://8bitscoding.github.io/cpp-dp-s6-6/)

---

* [옵저버 패턴(Observer Pattern)](https://8bitscoding.github.io/cpp-design-observer/)
* [팩토리 패턴(Factory Pattern)](https://8bitscoding.github.io/cpp-design-factory/)