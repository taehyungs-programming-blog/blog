---
title: "(C++) auto"
permalink: cpp/auto/                # link 직접 지정
#toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-02-06 00:00:00 -0000
last_modified_at: 2020-10-06 00:00:00 -0000
sidebar:
  title: "목차"
  nav: cpp
tag:
  - cpp
category:
  - auto
classes: wide
excerpt: ""
header:
  teaser: /file/image/cpp-page-teaser.gif
---

```cpp
// 타입이 추론되는 규칙에 대해 알아보자.
int main()
{
  int n = 10;
  int& r = n;
  
  auto a = r;   // a는 int인가 int&인가
  a = 30;
  
  cout << n << endl;    // 
}
```

```cpp
int main()
{
  int n = 10;
  int& r = n;
  const int c = n;
  const int& cr = c;
  
  auto a1 = n;      // int
  auto a2 = r;      // int (int&가 아님을 주의하자, auto는 값 복사이기에 r의 값을 확인)
  auto a3 = c;      // int (const속성은 무시된다.)
  auto a4 = cr;     // int
  
  auto& a5 = n;     // auto : int, a5 : int&
  auto& a6 = r;     // auto : int, a6 : int&
  auto& a7 = c;     // auto : const int, a7 : const int&
  auto& a8 = cr;    // auto : const int, a8 : const int&
  
  const char* s1 = "hello";   // s1 자체는 const아님
                              // s1을 따라가면 const
  auto a9 = s1;     // const char*
  
  const char* const s2 = "hello";
  auto a10 = s1;    // const char*
}
```

---

```cpp
auto text() {
  return 7;   // 이런것도 가능
}

auto text() -> int {
  return 7;   // 직접 지정도 가능
}

// 보통은 아래와 같이 쓴다
template <class T>
auto test(T value) -> decltype(value) {
  return value;
}

template <class T, class S>
auto test(T value, S value2) -> decltype(value + value2) {
  return value + value2;
}

int get() {
  return 999;
}

// 함수를 리턴할때 auto를 쓰고 싶다면 이렇게 쓰자
auto test2() -> decltype(get()) {
  return get();
}
```

---

## 추가

* [참고](https://www.youtube.com/watch?v=U6mgsPqV32A&list=PL5jc9xFGsL8FWtnZBeTqZBbniyw0uHyaH)

```cpp
std::vector<int> vec = {2, 3, 4, 5};

// C++ 03
for (std::vector<int>::iterator it = vec.begin(); it != vec.end(); ++it)
  m_vec.push_back(*it);

// C++ 11
for (auto it = vec.begin(); it != vec.end(); ++it)
  m_vec.push_back(*it);
```