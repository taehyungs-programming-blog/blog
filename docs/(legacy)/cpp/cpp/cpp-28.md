---
layout: default
title: "28. std::mutable"
parent: "(C++) 문법"
grand_parent: (C++)
great_grand_parent: "Legacy Archive"
nav_order: 3
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

{% raw %}

* [참고사이트 🌎](https://modoocode.com/253)

🐳 아래와 같은 함수가 있다고 가정해 보자.

```cpp
class A {
    int data_A;
    int data_B;

public:
    A(int data) : data_(data) {}

    void DoSomething(int x) const {
        // const 선언으로 불가능!
        data_A = x;  
        data_B = x;
    }
```

🐳 만약 `DoSomething`함수에서 `data_A`를 꼭 바꿔야만 하는 상황이 온다면??

```cpp
class A {
    mutable int data_A;
    int data_B;

public:
    A(int data) : data_(data) {}

    void DoSomething(int x) const {
        data_A = x;     // Okay
        data_B = x;     // Error
    }
```

🐳 이게 효율적인가??<br>
🐳 `const`선언의 본질적 의미는 해당 클래스(오브젝트)에 아무런 영향을 끼치지 않겠다는 의미이다.<br>
🐳 예를들어 `GetXXX()`과 같은 함수에선 데이터를 읽어오긴 하나 해당 오브젝트에는 아무런 영향을 줘선안된다. 하지만 부분적으로 `const`가 없는 표현이 필요할 경우 `mutable`을 사용하도록 하자.

{% endraw %}