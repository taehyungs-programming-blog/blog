---
layout: default
title: "6. IOCP"
parent: "(winsock)"
grand_parent: (C++)
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* IOCP의 특징
    * OS가 I/O를 처리하기에 고속 입/출력이 가능
    * OS는 스레드 풀을 관리하여 일종의 Proactor(미리 준비된) 방식으로 처리
    * CallBack 방식으로 동작(내가 별도의 Thread를 돌면서 확인할 필요가 없다)

