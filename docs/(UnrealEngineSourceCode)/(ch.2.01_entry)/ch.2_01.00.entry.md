---
layout: default
title: "([Network] 01.SocketSubsystemModule)"
parent: "(UE SourceCode 분석 🤖)"
has_children: true
nav_order: 3
---

* Lyra에서 통신을 하게 될 텐데 Lyra를 세팅하는 방법은 생략한다.

---

## Tip) Development 빌드 했는데 좀 더 디테일 한 디버그를 원한다?

* Debug로 재빌드?
    * 너무 오래 걸리고 오히려 디버깅 방해됨

```cpp
UE_DISABLE_OPTIMIZATION
// ~
UE_ENABLE_OPTIMIZATION
// 을 활용해 보자
```

---

## Tip) UDP 메시지만 (패킷 송수신만) 디버깅 하고 싶을때?

* 브레이크 걸어보면 쓸데없는 UE 다른 메시지가 브레이크 포인트에 많이 걸린다.
    * UE내부적으로 다른 네트워크 작업이 많음(주소알리기 핑 날리기 등등...)
* Project Settings -> Plugins -> UDP Messaging -> Transport -> Enable Transport를 False로

