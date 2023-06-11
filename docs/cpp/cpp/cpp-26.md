---
layout: default
title: "26. [C++17] std::variant"
parent: (C++)
grand_parent: (C++)
nav_order: 3
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

{% raw %}

* [참고사이트 🌍](https://modoocode.com/309)

---

🍅 **결론** : 변수의 타입을 One-of로 지정할 수 있다.

```cpp
// v 는 이제 int
std::variant<int, std::string, double> v = 1;

// v 는 이제 std::string
v = "abc";

// v는 이제 double
v = 3.14;
```

🍅 너무 좋은데?? 🐥 초보자 입장에선 왜 좋은지 이해가 잘 되지 않을텐데 아래를 보자.

```cpp
class Data {};
class A : public Data {};
class B : public Data {};

std::unique_ptr<Data> GetDataFromDB(bool is_a) {
    // 특정 조건을 만족시 A 혹은 B 클래스를 리턴하는 함수가 있다 가정하자
    // 기존에는 이런 함수를 만드려먼 A, B의 부모로 Data를 두고 Data형으로 리턴했어야 한다.
  if (is_a) {
    return std::make_unique<A>();
  }
  return std::make_unique<B>();
}
```

```cpp
#include <iostream>
#include <memory>
#include <variant>

class A {
 public:
  void a() { std::cout << "I am A" << std::endl; }
};

class B {
 public:
  void b() { std::cout << "I am B" << std::endl; }
};

std::variant<A, B> GetDataFromDB(bool is_a) {
  if (is_a) {
    return A();
  }
  return B();
}

int main() {
  auto v = GetDataFromDB(true);

  std::cout << v.index() << std::endl;
  std::get<A>(v).a();  // 혹은 std::get<0>(v).a()
}
```

{% endraw %}