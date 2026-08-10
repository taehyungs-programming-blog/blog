---
layout: default
title: "33. std::vector vs std::array"
parent: "(C++) 문법"
grand_parent: (C++)
great_grand_parent: "Legacy Archive"
nav_order: 5
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

{% raw %}

* [참고 사이트 🌍](https://snowdeer.github.io/c++/2019/01/11/difference-between-vector-and-array/)

* **std::vector**
    * 동적 크기 변경 : 가능 but 하지않는 것을 추천
    * 메모리 사용 : 실제 할당된 메모리 보다 조금 더 많이 사용
    * 데이터 접근 시간 : array에 비해 느림(index를 동적 생성하는 시간, 사실 큰 의민 없다)
* **std::array**
    * 동적 크기 변경 : 불가능
    * 메모리 사용 : vector에 비해 덜 사용(동적으로 재할당 하지않기에)
    * 데이터 접근 시간 : vector에 비해 빠름

{% endraw %}