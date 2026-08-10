---
layout: default
title: "29. std::explicit"
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

```cpp
class MyString {
    char* string_content;  // 문자열 데이터를 가리키는 포인터
    int string_length;     // 문자열 길이

    int memory_capacity;

public:
    // capacity 만큼 미리 할당함.
    MyString(int capacity);

    // 문자열로 부터 생성
    MyString(const char* str);

    // 복사 생성자
    MyString(const MyString& str);
```

```cpp
MyString("abc");                // MyString(const char* str); 호출
MyString s = MyString("abc");   // MyString(const MyString& str); 호출
MyString(5);                    // MyString(int capacity); 호출

// 여기까진 의도한 대로 동작

void DoSomethingWithString(MyString s) {
  // Do something...
}

DoSomethingWithString(5);   // MyString(int capacity); 호출
// 문제는 이걸 DoSomethingWithString함수를 개발한 개발자가 의도한 것이 맞냐는 것이다.
// Capacity를 5를 주려했던 것인지 text 5를 주려했던것인지???
// 이런 의도치 않는 상황을 막고 싶다면?? -> explicit
```

```cpp
class MyString {
    // ...

public:
    explicit MyString(int capacity);

    // ...
```

```cpp
// 추가적으로 이런것도 막아준다.
MyString s = "abc";  // MyString s("abc") 호출
MyString s = 5;      // Error
```

{% endraw %}