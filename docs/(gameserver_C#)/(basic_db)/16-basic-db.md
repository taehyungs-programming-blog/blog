---
layout: default
title: "16. [문법] UNION"
parent: "(DB 연결 기초)"
grand_parent: "(GameServer C# 🎯)"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* 복수의 테이블을 다루어 보자

```sql
-- 커리어 평균 연봉이 3000000 이상인 선수 playerID
SELECT playerID, AVG(salary)
FROM salaries
GROUP BY playerID
HAVING AVG(salary) >= 3000000
```

* `WHERE AVG(salary) >= 3000000`를 안쓴이유? 
    * 어찌보면 당연, GROUP BY 되어 이기에 AVG로 구해야함.
    * WHERE을 쓰지못한다

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/database/basic-16-1.png"/>
</p>

```sql
-- 12월에 태어난 playerID
SELECT playerID, birthMonth
FROM players
WHERE birthMonth = 12
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/database/basic-16-2.png"/>
</p>

```sql
-- 평균 연봉이 3000000이거나 12월에 태어난 선수
-- 위 두 결과를 합치는걸 원한다 -> UNION
SELECT playerID
FROM salaries
GROUP BY playerID
HAVING AVG(salary) >= 3000000
UNION
SELECT playerID
FROM players
WHERE birthMonth = 12
-- 단, 중복은 제외된다
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/database/basic-16-3.png"/>
</p>

```sql
UNION ALL -- 중복을 제외하고 싶지않다면 ALL을 넣자
```