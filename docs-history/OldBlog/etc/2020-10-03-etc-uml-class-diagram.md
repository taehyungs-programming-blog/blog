---
title: "(UML) Class Diagram"
permalink: etc/uml/class-diagram/                # link 직접 지정
#toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-10-03 00:00:00 -0000
last_modified_at: 2020-10-03 00:00:00 -0000
tag:
  - UML
category:
  - class diagram
sidebar:
  - title: ""
  - nav:
classes: wide
excerpt: ""
header:
  teaser: /file/image/etc-page-teaser.gif
---

* [참고사이트](https://gmlwjd9405.github.io/2018/07/04/class-diagram.html)

---

## class

**UML 클래스의 표현**

- 가장 윗부분: 클래스 이름
- 중간 부분: 변수
- 마지막 부분: 함수

![](/file/image/class-digram-1.png)

**접근제어자 표현**

- + : public
- - : private
- # : protected

![](/file/image/class-digram-2.png)

---

## 관계

**화살표**

- 한쪽은 알지만 다른 한 쪽은 모른다

![](/file/image/class-digram-3.png)

**실선**

- 양쪽이 서로를 안다

![](/file/image/class-digram-4.png)

**다중성 표시**

![](/file/image/class-digram-5.png)

- 아무런 표시가 없으면 1:1

**연관클래스**

![](/file/image/class-digram-6.png)

---

## 일반화 관계

- 상속이라 생각 더 자세한건 참고페이지에서

![](/file/image/class-digram-7.png)

---

## 집합 관계

- 한 객체가 다른 객체를 포함
- 빈 마름모, 채운 마름모 차이는
- 빈 마름모 : 라이프 사이클이 별도
- 채운 마름모 : 라이프 사이클이 같이 간다

![](/file/image/class-digram-8.png)

---

이정도만 알아도 충분