---
layout: default
title: "3. SO_REUSEADDR : 서버의 재 시작 방법"
parent: "(winsock)"
grand_parent: (C++)
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* IP/Port를 열어둔 상태로 서버가죽었다.
* 그런데 IP/Port가 열려있기에 다시 Socket bind를 하지못한다면?
    * 이 상황을 방지하기위해서 `SO_REUSEADDR`가 있다
* 궁금한건 그럼 Process 두 개가 동시에 같은 IP/Port를 bind한다면? 
    * 첫 번째 실행한 Process로 데이터가 간다
    * 만약 첫 번째 Process가 종료되면 두 번째 Process가 동작함(에러처리를 좀 해줘야 함.)

```cpp
BOOL bOption = TRUE;
if (::setsockopt(hSocket, SOL_SOCKET,
    SO_REUSEADDR, (char*)&bOption, sizeof(BOOL)) == SOCKET_ERROR) {
    return 0;
}
```

