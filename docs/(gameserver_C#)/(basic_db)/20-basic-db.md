---
layout: default
title: "20. [문법] 윈도우 함수"
parent: "(DB 연결 기초)"
grand_parent: "(GameServer C# 🎯)"
nav_order: 3
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* 여기서 말하는 윈도우는 OS가 아님
* **윈도우 함수** - 행들의 서브 집합을 대상으로, 각 행별로 계산을 해서 스칼라(단일 고정)값을 출력하는 함수, `GROUPING(SUM, COUNT ...)`과 유사
    * 뭔 소린가? - 행 별로 연산의결과를 출력한단 말! (그냥 아래를 보자)

```sql
USE BaseballData

SELECT playerID
FROM salaries
GROUP BY playerID
ORDER BY MAX(salary) DESC
/*
    여기서 문제가 SELECT로 playerID는 볼 수 있지만 
    salary, teamID와 같은 다른 정보는 볼 수 없음
    윈도우 함수로 해결해 보자.
*/
```

```sql
-- OVER([PARTITION] [ORDER BY] [ROWS])
-- 전체 데이터를 연봉 순서로 나열후 순위 표기
SELECT *,
    ROW_NUMBER() OVER (ORDER BY salary DESC) AS 'Num',   
    -- 행 번호
    RANK() OVER (ORDER BY salary DESC) AS 'Rank(1)',         
    -- 랭킹 (공동 1위 후 3위로 처리)
    DENSE_RANK() OVER (ORDER BY salary DESC) AS 'Rank(2)',   
    -- 랭킹 (공동 1위 후 2위 처리)
    NTILE(100) OVER (ORDER BY salary DESC) AS 'Percent'     
    -- 백분률로 표기
FROM salaries
```

* 좀 해석을 해보자면 ...
    * `ROW_NUMBER()` - 행 번호를 넣어주세요
    * 단, `(ORDER BY salary DESC)` 정렬은 salary를 기준으로 합니다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/database/basic-20-1.png"/>
</p>

```sql
-- playerID 별 순위를 따로 정리
SELECT *,
    ROW_NUMBER() OVER (PARTITION BY playerID ORDER BY salary DESC)
FROM salaries
ORDER BY playerID
```

* 역시 해석을 좀 하자면
    * `ROW_NUMBER()` - 행 번호를 넣어주세요
    * 단, 부분적 으로 `PARTITION BY playerID`
    * 정렬은 salary를 기준으로 `ORDER BY salary DESC`

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/database/basic-20-2.png"/>
</p>

```sql
-- LAG(바로 이전), LEAD(바로 다음)
SELECT *,
    RANK() OVER (PARTITION BY playerID ORDER BY salary DESC),
    LAG(salary) OVER (PARTITION BY playerID ORDER BY salary DESC) AS prevSalary,
    LEAD(salary) OVER (PARTITION BY playerID ORDER BY salary DESC) AS nextSalary
FROM salaries
ORDER BY playerID
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/database/basic-20-3.png"/>
</p>

---

* 활용해 보기

```sql
-- FIRST_VALUE, LAST_VALUE
-- FRAME(범위) : First ~ current
SELECT *,
    RANK() OVER (PARTITION BY playerID ORDER BY salary DESC),
    FIRST_VALUE(salary) OVER (PARTITION BY playerID 
                                ORDER BY salary DESC 
                                ROWS BETWEEN UNBOUNDED PRECEDING AND CURRENT ROW) AS best,
    LAST_VALUE(salary) OVER (PARTITION BY playerID 
                                ORDER BY salary DESC
                                ROWS BETWEEN CURRENT ROW AND UNBOUNDED FOLLOWING) AS worst,
FROM salaries
ORDER BY playerID
```