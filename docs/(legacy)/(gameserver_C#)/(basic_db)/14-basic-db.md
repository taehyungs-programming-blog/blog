---
layout: default
title: "14. [문법] 정규화"
parent: "(DB 연결 기초)"
grand_parent: "(GameServer C# 🎯)"
great_grand_parent: "Legacy Archive"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* **정규화** - 테이블을 올바르게 변경하고 **분할**하는 것.
* **Q.** - 왜 분할해야할까?? -> 우선 아래와 같은 DB가 있다고 가정하자

```
** TABLE character **

| ID | name | class | level | inventory |
------------------------------------------
| 1  | 태형 | 백수   | 1     | 연필, 의자 |
...
```

* inventory의 아이템이 변경될 경우 string을 파싱하는 수고로움이 발생한다-> 이런경우 DB의 분할이 필요하다.

```
** TABLE inventory **

| ID | item_id | item_name | item_cnt |
---------------------------------------
| 1  | 1       | 연필      | 1         |
| 1  | 2       | 의자      | 1         |
```

* 이러면 모두 해결? 
* 아직 문제가 있다. item_id, item_name은 중복이다.

```
** TABLE itemInfo **
| item_id | item_name |
-----------------------
| 1       | 연필       |
| 2       | 의자       |
```

```
** TABLE inventory **

| ID | item_id | item_cnt |
---------------------------------------
| 1  | 1       | 1         |
| 1  | 2       | 1         |
```

* 결론적으로 데이터의 중복을 없애기 위해 노력하는 것이다.