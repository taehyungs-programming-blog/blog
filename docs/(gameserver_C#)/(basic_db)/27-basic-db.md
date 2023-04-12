---
layout: default
title: "27. [튜닝] Nested Loop 조인"
parent: "(DB 연결 기초)"
grand_parent: "(GameServer C# 🎯)"
nav_order: 3
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* 지금 부터 조인에 대해 알아보자.

* **조인의 종류**
    * **Nested Loop 조인**
    * **Mrege 조인**
    * **Hash 조인**

* 우선 간단히 어디서 쓰이나 보자.

```sql
USE BaseballData

-- Merge Join
    -- 두 Table을 Merge한다고 Merge Join이란 이름이 붙었나?
SELECT *
FROM players AS p
    INNER JOIN salaries AS s
    ON p.playerID = s.playerID;
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/database/basic-27-1.png"/>
</p>

```sql
-- 중첩 루프(Nested Loop)
    -- 최상단 5개만 뽑으려면 반복문을 쓸텐데 그 과정에서 Nested Loop Join이란 이름이 붙었나?
SELECT TOP 5 *
FROM players AS p
    INNER JOIN salaries AS s
    ON p.playerID = s.playerID;
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/database/basic-27-2.png"/>
</p>

```sql
-- Hash Join
    -- 이건 상상이 안되는군 ...
SELECT *
FROM salaries AS s
    INNER JOIN teams AS t
    ON t.teamID = s.teamID;
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/database/basic-27-3.png"/>
</p>

---

## Nested Loop

* 코드로 보면 이중 For문이라 생각하자
* 읔! 이중 For문 ... 무조건 느리지 않나? 
    * 👉 꼭 그렇지는 않다. 특정 개수를 찾을경우(`TOP 5`) 오히려 빠를 수 있음.

* **Nested Loop 동작원리**
  * 먼저 액세스한 (OUTTER)테이블의 로우에 순차적 접근
  * Index Seek를 통해 (INNER)테이블에 랜덤 액세스
  * 따라서, (INNER)테이블에 인덱스가 없다면 시간이 오래걸린다
  * (추가) 순차적으로 돌기에 정해진 갯수를 검색하는데 좋다.

* 예를들어

```sql
SELECT TOP 5 *
FROM players AS p               -- INNER
    INNER JOIN salaries AS s    -- OUTTER
    ON p.playerID = s.playerID;
```

* OUTTER를 TOP 5를 뽑는다
* OUTTER를 기반으로 INNER의 Index Seek를 돈다