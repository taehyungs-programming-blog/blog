---
title: "(c++20) std::optional"
permalink: cpp/optional/                # link 직접 지정
#toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2021-01-26 00:00:00 -0000
last_modified_at: 2021-01-26 00:00:00 -0000
sidebar:
  title: "목차"
  nav: cpp
tag:
  - cpp
category:
  - optional
classes: wide
excerpt: ""
header:
  teaser: /file/image/cpp-page-teaser.gif
---

* [참고사이트](https://modoocode.com/309)

```cpp
#include <iostream>
#include <map>
#include <string>
#include <utility>

std::optional<std::string> GetValueFromMap(const std::map<int, std::string>& m,
                                           int key) {
  auto itr = m.find(key);
  if (itr != m.end()) {
    return itr->second;
  }

  // nullopt 는 <optional> 에 정의된 객체로 비어있는 optional 을 의미한다.
  return std::nullopt;
}

int main() {
  std::map<int, std::string> data = {
    {1, "hi"}, {2, "hello"}, {3, "hiroo"}
    };
  std::cout << "맵에서 2 에 대응되는 값은? " << GetValueFromMap(data, 2).value()
            << std::endl;
  std::cout << "맵에 4 는 존재하나요 " << std::boolalpha
            << GetValueFromMap(data, 4).has_value() << std::endl;
}
```