---
layout: default
title: "45. Object-C) Core Foundation"
parent: "(C++) 문법"
grand_parent: (C++)
nav_order: 7
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## ARC와 CF의 메모리 관리 차이

* **ARC(Automatic Reference Counting)**: Swift나 Objective-C 객체를 자동으로 참조 카운트 증가/감소해주고, 스코프를 벗어나면 해제
* **CF(Core Foundation)**: 참조 카운트를 직접 CFRetain / CFRelease로 관리

> 이 둘 사이에 객체를 넘겨줄 때, 소유권(Ownership) 규칙이 맞지 않으면 객체가 예상보다 일찍 해제되거나, 반대로 해제되지 않아 메모리 누수가 생길 수 있음(주의)

---

## CFBridgingRetain의 역할

* CFBridgingRetain(obj)은 ARC 객체를 CF 스타일 소유권으로 변환합니다.
* ARC가 관리하는 객체를 retain 해서 참조 카운트를 +1
* 반환 값은 CFTypeRef (또는 void*)로, 이제 ARC가 자동 해제하지 않음
* Core Foundation 규칙에 따라 나중에 직접 CFRelease() 해야 함

```c
void *ptr = (void*)CFBridgingRetain([[AAA alloc] init]);
// ARC 관리 X, Core Foundation 스타일로 들고 있음

AAA *obj = (__bridge AAA*)ptr;
// 사용...

CFRelease(ptr); // 직접 해제
```

### `__bridge `는 뭐지?

* __bridge는 Objective-C ARC에서 Objective-C 객체 포인터와 Core Foundation 포인터 간 캐스팅을 할 때 사용하는 브리지 캐스트(bridge cast) 문법

* Objective-C의 ARC는 자동으로 retain/release를 관리
* 하지만 Core Foundation(CFTypeRef)나 C API는 ARC 관리 대상이 아니기 때문에, 두 세계를 오갈 때 소유권(ownership) 규칙을 명확하게 표시
* 그때 쓰는 것이 __bridge, __bridge_retained, __bridge_transfer입니다.

| 문법                  | 소유권 변화       | 설명                                             |
| ------------------- | ------------ | ---------------------------------------------- |
| `__bridge`          | 없음           | 참조 카운트 변동 없이 타입만 변환                            |
| `__bridge_retained` | ARC → CF로 이동 | ARC 참조 유지 +1, ARC 관리에서 제외 → 나중에 `CFRelease` 필요 |
| `__bridge_transfer` | CF → ARC로 이동 | CF 소유권을 ARC로 이전, ARC가 해제 담당                    |

```c
// ARC 객체 → CF 포인터 (소유권 유지)
CFStringRef cfStr = (__bridge CFStringRef)@"Hello"; // CFRelease 필요 없음

// ARC 객체 → CF 포인터 (소유권 이전)
CFStringRef cfStr2 = (__bridge_retained CFStringRef)[NSString stringWithUTF8String:"Hi"];
CFRelease(cfStr2); // 직접 해제 필요

// CF 포인터 → ARC 객체 (소유권 이전)
NSString *str = (__bridge_transfer NSString *)CreateCFString(); // ARC가 자동 해제

```

---

## CF를 왜 쓰지? 그냥 내가 메모리 관리하면 되지 않나?

* ARC는 필수는 아니다. 
* 해당 파일(또는 타깃)을 MRC(-fno-objc-arc) 로 빌드하면 직접 retain/release로 관리해도 됩니다. 이 경우 CFBridgingRetain이 필요 없음
* 다만 ARC를 쓰는 프로젝트에서 C++/CF 쪽으로 소유권을 넘겨 오래 들고 있어야 할 때, 정확하고 안전하게 하려면 CFBridgingRetain/Release가 가장 단순하고 실수 적은 방법

* “CF가 메모리를 관리해준다”는 개념은 아닙니다. CF도 결국 레퍼런스 카운트이며, CFBridgingRetain은 *“이 포인터는 지금부터 너(CF/C++측)가 들고 있어. 나중에 CFRelease로 돌려줘”*를 명시해주는 소유권 이전(ownership transfer) 도구

```c
// .mm (ObjC++)
WebSocketMacOS* obj = [[WebSocketMacOS alloc] initWithURL:Url]; // retain count = 1
this->WebSocket = obj;  // 그냥 보관 (void*로 캐스팅해도 됨)

// 사용할 때
WebSocketMacOS* ws = (WebSocketMacOS*)this->WebSocket;
[ws sendText:@"hi"];

// 수명 끝
[(id)this->WebSocket release];
this->WebSocket = nullptr;

```