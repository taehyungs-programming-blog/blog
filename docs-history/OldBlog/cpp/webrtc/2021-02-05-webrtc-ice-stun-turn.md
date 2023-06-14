---
title: "(WebRTC) ICE, STUN, TURN"
permalink: cpp/nwebrtc/ice-stun-turn/                # link 직접 지정
#toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2021-02-05 00:00:00 -0000
last_modified_at: 2021-02-05 00:00:00 -0000
sidebar:
  title: "목차"
  nav: cpp
tag:
  - cpp
  - webrtc
category:
  - ice
  - turn
  - stun
classes: wide
excerpt: ""
header:
  teaser: /file/image/cpp-page-teaser.gif
---

* [참고사이트](https://brunch.co.kr/@linecard/156) : ICE, STUN, TURN이론
* [참고사이트](https://www.html5rocks.com/ko/tutorials/webrtc/infrastructure/) : 전반적 WebRTC 통신 정리

---

* STUN : 내부 IP가 아닌 자신의 외부 IP를 받는다
* TURN : Peer간 직접통신을 맺는다. 실패시 Peer간 릴레이를 담당한다.