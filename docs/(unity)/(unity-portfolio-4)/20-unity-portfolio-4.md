---
layout: default
title: "20. 대형몬스터 구현"
parent: "(포트폴리오 4 - 방치형 게임)"
grand_parent: "(Unity ✨)"
nav_order: 3
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌎](https://github.com/Arthur880708/Unity.IncrementalGame.Example/tree/8)

---

# TODO : 타일을 2개 이상 차지하는 대형몬스터

* CanGo를 여러타일에 대해 계산
    * CanGo_Internal

---

# TODO : A* 알고리즘

* 대형몬스터의 중점을 위주로 찾는데 그게 맞나?
    * 문제는 보스몬스터 주변에 내 캐릭들이 들어가는경우 빈자리를 찾아가게 해야한다

```
 x x x
 x O v
 x v v

v - 여기에 내 캐릭들이 들어가게 해야함.
```

* 현재는 depth를 수정해서 보완 (이후 더 고려할 것)
