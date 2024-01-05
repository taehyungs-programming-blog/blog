---
layout: default
title: "40. std::exchange"
parent: "(C++) 문법"
grand_parent: (C++)
nav_order: 6
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* std::exchange는 C++ 표준 라이브러리의 유틸리티 함수로, 객체의 값을 새로운 값으로 교체하고, 교체되기 전의 원래 값을 반환하는 기능을 수행합니다. 
* 이 함수는 `<utility>` 헤더 파일에 정의되어 있으며, 두 개의 매개변수를 받습니다: 교체될 객체의 참조와 교체할 새로운 값입니다.

```cpp
#include <utility>

int main() {
    int a = 5;
    int b = 10;

    // a의 값을 b로 교체하고, 교체 전의 a 값을 old_a에 저장
    int old_a = std::exchange(a, b);

    // 이제 a는 10, old_a는 5
}
```