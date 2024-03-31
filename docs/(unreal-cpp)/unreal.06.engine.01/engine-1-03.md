---
layout: default
title: "03. UEngine"
parent: "(Engine Source Code 분석 1)"
grand_parent: "(Unreal C++ 🚀)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* UEngine의 자녀인 UEditorEngine를 살펴보자

## UEngine

```cpp
class UEngine : public UObject, public FExec
// class UEditorEngine : public UEngine
```

---
---

## TIPS) `: 1` 과 같은 표현은 뭘 의미할까?

```cpp
// EX

typedef struct _CUVIDVP9PICPARAMS
{
    // ...

    // short를 모두 쓰지않고 1비트만 쓴다를 의미
    unsigned short frameType : 1;
    unsigned short showFrame : 1;

    // 이런경우 3비트겠지?
    unsigned short profile : 3;
    unsigned short frameContextIdx : 2;
```

---

## `FActorTickFunction`

