---
title: "Windows : 인바운드, 아웃바운드 규칙이란"
permalink: windows/inoutbound/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-04-17 00:00:00 -0000
last_modified_at: 2020-04-17 00:00:00 -0000
sidebar:
  title: "etc"
  nav: etc
---

고급 방화벽에서 확인할 수 있다.

![](/file/image/win-firewall.PNG)

* 인바운드 규칙
    - 외부(인터넷)에서 내부(내 PC)로 들어오는 규칙
    - Windows의 기본 인바운드 규칙이 모든접속 **차단**이기에 추가되는 규칙은 보통 새로운 포트를 추가할때 사용된다.
* 아웃바운드 규칙
    - 내부(내 PC)에서 외부(인터넷)로 규칙
    - Windows의 기본 아웃바운드 규직이 모든접속 **허용**이기에 추가되는 경우는 특정 포트를 사용해야하는 경우 추가되어 사용된다.

* [참고사이트](https://kcmschool.com/77)