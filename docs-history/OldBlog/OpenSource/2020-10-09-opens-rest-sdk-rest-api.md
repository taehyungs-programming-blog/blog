---
title: "(OpenSource - REST SDK) REST API란?"
permalink: opens/rest-sdk/rest-api/                # link 직접 지정
#toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-10-09 00:00:00 -0000
last_modified_at: 2020-10-09 00:00:00 -0000
tag:
  - OpenSource
  - REST SDK
category:
  - REST API
sidebar:
  - title: ""
  - nav:
classes: wide
excerpt: ""
header:
  teaser: /file/image/OpenS-page-teaser.gif
---

* [참고사이트](https://www.youtube.com/watch?v=iOueE9AXDQQ)

다른 설명보다 유투브 설명이 너무 좋아서 유투브를 참고사이트로 넣음

---

REST API는 일종의 데이터 송/수신의 형식이라 보면 된다.

여기서 드는 의문은

* 1. REST 형식은 뭘까?

특정한 포멧으로 정리해 송/수신단에서 쉽게 읽고 쓸 준비를 한 것이라 보면 된다.<br>
대표적 장점은 REST 형식만 보고 어떤 요청을 하는 것인지 대충 추론이 가능하다는 점이다(주소만봐도 대충알어! <-이게 핵심)

* 2. 그럼 API는 뭘까?

API(Application Programming Interface) : 소프트웨어가 다른 소프트웨어로 부터 지정된 형식으로 요청, 명령을 받을 수 있는 수단

* 정리하자면 ...

REST라는 형식으로 API 소프트웨어간 통신을 하는 녀석을 우린 REST API라고 부르자 라고 정의한다

---

여기까지 REST API의 명사에 대한 정의는 끝났고 좀 더 자세한 설명에 들어가기 전 몇 가지 개념을 정리하고 가자

* CRUD 란?

Create(C), Read(R), Update(U), Delete(D) 를 의미하며 

http로 명령을 GET, POST, PUT, PATCH, DELETE ... 으로 보낼 수 있을것이다. 이렇게 보내는 http메시지를 아래와 같이 REST API로 매핑시킬수 있다. 어떤 메소드를 사용할지를 정리한 것이라 생각하자

GET - READ
POST - CREATE
PUT or PATCH - UPDATE
DELETE - DELETE