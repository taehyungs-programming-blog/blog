---
layout: default
title: "17. [문법] JOIN"
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

* 우선 테스트용 테이블을 만들어보자.

```sql
USE GameDB

CREATE TABLE testA
(
    a INTEGER
)

CREATE TABLE testB
(
    b VARCHAR(10)
)

INSERT INTO testA VALUES(1)
INSERT INTO testA VALUES(2)
INSERT INTO testA VALUES(3)

INSERT INTO testB VALUES('A')
INSERT INTO testB VALUES('B')
INSERT INTO testB VALUES('C')

SELECT *
FROM testA
SELECT *
FROM testB
```

```sql
-- CROSS JOIN(중요도 낮음)
-- 두 가지 방법으로 표현가능
SELECT *
FROM testA
    CROSS JOIN testB

SELECT *
FROM testA, testB
-- 3 * 3 = 9개의 데이터가 나오게 된다.(아래참고)
```

* 사실상 쓸일이 그렇게 없다;;

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/database/basic-17-1.png"/>
</p>

```sql
SELECT *
FROM testA
    INNER JOIN testB
-- error - 뭘 기준으로 JOIN할지 모른다.
-- primary key가 없음.
```

```sql
USE BaseballData

SELECT *
FROM players
ORDER BY playerID
SELECT *
FROM salaries
ORDER BY playerID
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/database/basic-17-2.png"/>
</p>

```sql
-- INNER JOIN(중요도 높음)
-- 두 개의 테이블을 가로로 결합, 결합 기준을 ON으로
SELECT *
FROM players AS p
    INNER JOIN salaries AS s
    ON p.playerID = s.playerID

-- playerID가 players, salaries양쪽에 다 있고 일치하는 애들의 결합
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/database/basic-17-3.png"/>
</p>

```sql
-- OUTER JOIN
-- LEFT, RIGHT - 어느 한쪽에만 존재하는 데이터의 정책

SELECT *
FROM players AS p
    LEFT JOIN salaries AS s
    ON p.playerID = s.playerID
-- playerID가 왼쪽(players)에 있으면 무조건 표시
-- 오른쪽(salaries)에 있으면 오른쪽 정보는 NULL로 채움
```