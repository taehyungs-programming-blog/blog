---
layout: default
title: "08. Block vs Non-Block / Sync vs ASync의 정리"
parent: "(C++) Socket"
grand_parent: "Game Server 👾"
nav_order: 3
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* **Blocking** - 호출 후 대기
* **Non-Blocking** - 호출 후 대기 없음
* **Synchronous** - 호출 즉시 실행 후 응답(꼭 결과가 아니라 진행중을 리턴해도 됨)
* **Asynchronous** - 호출 즉시 실행되지 않아도 되며 Callback으로 응답

<br>

* 네 가지를 조합해 보면.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/iocp/iocp-33-1.png" style="border-radius:5%;border:1px solid #e6e1e8"/>
</p>

* 그림의 네 가지 모델이 모두 사용이 될까?
* Nope! 예를들어 Async-Blocking의 경우 Async(나중에 실행해도 된다.) + Blocking(그런데 난 널 기다릴께) 앞뒤가 안맞는 말이이다.
* 실제로 많이 사용되는 것은 Sync-Blocking, Async-NonBlocking정도라고 볼 수 있다.