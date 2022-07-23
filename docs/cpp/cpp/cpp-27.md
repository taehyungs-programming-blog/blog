---
layout: default
title: "27. std::tuple, structor binding"
parent: (C++)
grand_parent: C++
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

## (C++11) std::tuple

👾 다양한 데이터 타입을 좀 더 쉽게(`struct` 보다) 사용하기 위해 탄생됐다.

```cpp
#include <iostream>
#include <string>
#include <tuple>



int main() {
    // using struct
    struct Collection {
        int a;
        std::string s;
        double d;
    };

    Collection c;
    c.a = 1;
    c.s = "hi";
    c.d = 3.14;

    // using tuple
    std::tuple<int, double, std::string> tp;
    tp = std::make_tuple(1, 3.14, "hi");

    std::cout << std::get<0>(tp) << ", " << std::get<1>(tp) << ", " << std::get<2>(tp) << std::endl;
}
```

---

## (C++17) structor binding

👾 기존 struct의 불편한 점을 좀 더 개선하기 위해 나왔는데 불편한점이 뭔지 먼저 보자.

```cpp
#include <iostream>
#include <string>
#include <tuple>

std::tuple<int, std::string, bool> GetStudent(int id) {
  if (id == 0) {
    return std::make_tuple(30, "철수", true);
  } else {
    return std::make_tuple(28, "영희", false);
  }
}

int main() {
  auto student = GetStudent(1);

    // 이게 너무 단점이다 매번 get<0> ... 코드가 길어진다.
  int age = std::get<0>(student);
  std::string name = std::get<1>(student);
  bool is_male = std::get<2>(student);

  std::cout << "이름 : " << name << std::endl;
  std::cout << "나이 : " << age << std::endl;
  std::cout << "남자 ? " << std::boolalpha << is_male << std::endl;
}
```

```cpp
#include <iostream>
#include <string>
#include <tuple>

std::tuple<int, std::string, bool> GetStudent(int id) {
  if (id == 0) {
    return std::make_tuple(30, "철수", true);
  } else {
    return std::make_tuple(28, "영희", false);
  }
}

int main() {
  auto student = GetStudent(1);

  auto [age, name, is_male] = student;

  std::cout << "이름 : " << name << std::endl;
  std::cout << "나이 : " << age << std::endl;
  std::cout << "남자 ? " << std::boolalpha << is_male << std::endl;
}
```

{% endraw %}