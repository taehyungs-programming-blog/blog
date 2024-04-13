---
layout: default
title: "05. ULevel"
parent: "(Engine Source Code 분석 1)"
grand_parent: "(OpenSource👽)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

```cpp
// level은 actor의 집합이라 생각하자
class ULevel : public UObject
{
    // ...

    // ULevel도 역시 멤버변수를 확인하자
    TArray<TObjectPtr<AActor>> Actors;
        // level은 actor의 집합이라 생각하자 Actor를 보자면 다음강 참조!
}
```
