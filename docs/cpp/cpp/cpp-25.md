---
layout: default
title: "25. (C++17) std::optional"
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

🏄‍♂️ **결론부터 말하자면?** 그래서 어디 사용하나? 👉 데이터를 넘기는데 데이터가 들어있지 않은경우 NULL말고 다른 방법으로 데이터가 없다는 것을 알리고 싶다<br>
🏄‍♂️ 무슨말인가? 아래 예시를 보자

```cpp
#include <iostream>
#include <map>
#include <string>

// map key에 대응된 value를 리턴해주는 함수를 만들고자 한다.
std::string GetValueFromMap(const std::map<int, std::string>& m, int key) {
  auto itr = m.find(key);
  if (itr != m.end()) {
    return itr->second;
  }

    // 만약 대응되는 value가 없는 경우 아래와 같이 그냥 기본 생성자를 리턴해버리면 될까?
  return std::string();
}

int main() {
  std::map<int, std::string> data = {{1, "hi"}, {2, "hello"}, {3, "hiroo"}};
  std::cout << "맵에서 2 에 대응되는 값은? " << GetValueFromMap(data, 2)
            << std::endl;

    // 기본 생성자(빈값)이 넘어오기에 원하는 결과가 나오지 않는다.
  std::cout << "맵에서 4 에 대응되는 값은? " << GetValueFromMap(data, 4)
            << std::endl;
}
```

```cpp
// 1차 해결책

#include <iostream>
#include <map>
#include <string>

std::pair<std::string, bool> GetValueFromMap(
  const std::map<int, std::string>& m, int key) {
  auto itr = m.find(key);
  if (itr != m.end()) {
    return std::make_pair(itr->second, true);
  }

    // pair를 만들어서 second에 결과값을 넘겨보자.
  return std::make_pair(std::string(), false);
}

int main() {
  std::map<int, std::string> data = {{1, "hi"}, {2, "hello"}, {3, "hiroo"}};
  std::cout << "맵에서 2 에 대응되는 값은? " << GetValueFromMap(data, 2).first
            << std::endl;

    // 문제1) 결과값이 true, false냐에 따라 처리 방법이 다르다.
    // 문제2) std::string() 생성자는 한 번은 호출돼야 한다.(오버로드)
  std::cout << "맵에 4 는 존재하나요 " << std::boolalpha
            << GetValueFromMap(data, 4).second << std::endl;
}
```

```cpp
// 그래서 optional이 나왔다.

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
  std::map<int, std::string> data = {{1, "hi"}, {2, "hello"}, {3, "hiroo"}};
  std::cout << "맵에서 2 에 대응되는 값은? " << GetValueFromMap(data, 2).value()
            << std::endl;

    // 데이터가 있든 없든 같은 방법으로 처리가능
    // 디폴트 생성자가 호출되지 않아도 된다.
  std::cout << "맵에 4 는 존재하나요 " << std::boolalpha
            << GetValueFromMap(data, 4).has_value() << std::endl;

    // .has_value()로 데이터가 포함되었나 체크가능
}
```

{% endraw %}