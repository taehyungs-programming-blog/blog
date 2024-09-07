---
layout: default
title: "01-04. 왜 UE에서는 UDP를 기본으로 그리고 Socket 버전을 낮은 버전을 사용할까?"
parent: "([Network] 01.SocketSubsystemModule)"
grand_parent: "(UE SourceCode 분석 🤖)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## 1. WinSock을 쓰지않고 Linux Sock인 BSD를 쓰는 이유?

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/engine_net_study/01.04_01.png"/>
</p>

* 단순히 "싸서" 이다. Linux와 Windows 가격차이가 얼단 어마어마하다.
* 단, 개발용을 위해서 WinSock도 준비는 되어있다.

---

## 2. UDP(User Datagram Protocol), TCP(Transmission Control Protocol) 비교

* TCP	
    * Connection-oriented protocol
    * Congestion/Flow control
    * Ordered, Lower speed
    * Reliable data transmission
* UDP
    * Connection-less protocol
    * No Congestion/Flow control
    * Not ordered, Higher speed
    * Unreliable data transmission

* 상세 설명은 미 정리

---

## 3. 왜 UE는 UDP를 선택했을까?

* 결국 UDP를 안전하게 쓰는게 TCP인데, 이걸 꼭 TCP를 써야할까? 표준보다 내가 더 잘할수 있을꺼 같은데? 이 생각이 강한 듯 하다
* + UE자체적으로 Conjestion control, Flow control을 넣은부분이 있다. (결국 자기가 구현.)
* 결론) 빠르고 안전하게 내가 만들어 줄께 : UE UDP

---

## 4. 그럼 어떻게 UE는 안전하고 빠르게 만들어 줬을까?

* TCP가 느린 부분.
    * ACK가 오기 전 WindowSize이상 보낼수가 없다

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/engine_net_study/01.04_02.png"/>
</p>

* UDP가 빠른 부분.
    * 걍 보내!

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/engine_net_study/01.04_03.png"/>
</p>

* 그런데 요건 좀 애매한게 TCP가 발전하면 WindowSize를 거의 무한으로 할 수 있음.
    * 그냥 알고만 있자.

### MTU(Maximum Transmission Unit) Issue

* 내가 보낸 MTU가 아무리 1500이라도 Router의 MTU가 그 보다 낮다면?

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/engine_net_study/01.04_04.png"/>
</p>

* 패킷이 쪼개지개 되고, Latency의 원인이 된다.
* 그래서 일단 UE의 Maximum Packet Size는 1024이다.
    * Packet Fragmentation을 최대한 방지

