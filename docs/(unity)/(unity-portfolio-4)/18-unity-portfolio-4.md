---
layout: default
title: "18. 스탯 시스템"
parent: "(포트폴리오 4 - 방치형 게임)"
grand_parent: "(Unity ✨)"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌎](https://github.com/Arthur880708/Unity.IncrementalGame.Example/tree/7)

---

# TODO : Stat을 어떻게 접근하냐가 관건

* 직접접근시 어디서 수정했는지 혹은 원본값이 얼마인지 등 오류의 여지가 있음.
* `StatModifier`, `CreatureStat`
* Q) 정수 데이터는 int, float중 뭘로 넣어야 할까?
    * 정답은 없지만 float이 더 유리함
    * 왜지? int는 연산속도가 훨씬 더 빠를텐데? (부동소수점이 아니기에)
    * 여차피 퍼센트 연산등 연산이 들어가면 float으로 처리되고 자르는게 int이기때문.
    * 추가적으로 이후 스펙에 초당 0.1의 체력을 추가하게 해주세요 등 향후를 고려하는게 좋다

---


