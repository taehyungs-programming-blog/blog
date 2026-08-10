---
layout: default
title: "12. [문법] SUBQUERY"
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

* **SUBQUERY** : sql 명령문 안에서 지정하는 하부 SELECT

```sql
USE BaseballData

-- 연봉이 역대급으로 높은 선수 정보
SELECT TOP 1 *
FROM salaries
ORDER BY salary DESC
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/database/basic-12-1.png"/>
</p>

* 연봉이 1위인 rodrial01의 다른 정보를 보고싶다면?

```sql
SELECT *
FROM players
WHERE playerID = 'rodrial01'
-- rodrial01를 미리 알고 있어야 하나?
```

* **SUBQUERY**를 써보자

```sql
SELECT *
FROM players
WHERE playerID = (SELECT TOP 1 playerID FROM salaries ORDER BY salary DESC)
-- SUBQUERY가 먼저 실행되며 rodrial01를 먼저 받아오게 된다.
```

* 다중행일 경우 IN을 사용하자

```sql
SELECT *
FROM players
WHERE playerID IN (SELECT TOP 20 playerID FROM salaries ORDER BY salary DESC)
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/database/basic-12-2.png"/>
</p>

```sql
SELECT (SELECT COUNT(*) FROM players) AS playerCount, (SELECT COUNT(*) FROM batting) AS battingCount
```

* INSERT 내에서 사용해보자

```sql
SELECT *
FROM salaries
ORDER BY yearID DESC

INSERT INTO salareies
VALUE (2020, 'KOR', 'NL', 'name', (SELECT MAX(salary) FROM salaries))
-- 최대 연봉에 맞춰 연봉을 넣음

INSERT INTO salareies
SELECT 2020, 'KOR', 'NL', 'name', (SELECT MAX(salary) FROM salarie
-- 이것도 가능

INSERT INTO salareies_temp
SELECT yearID, playerID, salary FROM salaries
-- salaries 테이블에서 salareies_temp 테이블로 yearID, playerID, salary를 복사
```

---

## 상관 관계 서브쿼리

```sql
-- 포스트 시즌 타격에 참여한 선수들 목록
SELECT *
FROM player
WHERE playerID IN (SELECT playerID FROM battingpost)
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/database/basic-12-3.png"/>
</p>

* 상관 관계를 이용해 위 정보를 출력해보자

```sql
SELECT *
FROM player
WHERE EXISTS (SELECT playerID FROM battingpost WHERE players.playerID = battingpost.playerID)
-- EXISTS : 값이 있으면 출력 없으면 무시
```