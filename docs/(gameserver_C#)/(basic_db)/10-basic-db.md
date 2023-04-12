---
layout: default
title: "10. [문법] GROUP BY"
parent: "(DB 연결 기초)"
grand_parent: "(GameServer C# 🎯)"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

```sql
USE BaseballData

-- 복습
-- 2004년 보스턴 소속의 타격정보
SELECT *
FROM batting
WHERE yearId = 2004 AND teamId = 'BOS'

-- Q 2004년도에 가장 많은 홈런을 날린 팀은?
SELECT *
FROM batting
WHERE yearId = 2004
ORDER BY teamID
-- 팀별 홈런의 개수를 카운트 해야한다.
-- teamID를 기준으로 GROUP을 나누자
```

```sql
SELECT *   
FROM batting
WHERE yearId = 2004
GROUP BY teamID
/*
  에러가 날텐데 teamID로 GROUP BY할 기준이 없다.
  
  왜냐면 yearId 2004 기준으로 필터링했는데
  teamID는 중복된 값이 있을것이고 GROUP BY할 경우
  어떻게 GROUP BY를 할지 애매한것!
*/
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/database/basic-10-1.png"/>
</p>


* 팀까지 출력이 됨

```sql
SELECT teamID, COUNT(teamID) AS playerCount, SUM(HR) AS homeRuns
FROM batting
WHERE yearId = 2004
GROUP BY teamID
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/database/basic-10-2.png"/>
</p>

```sql
SELECT TOP 1 teamID, SUM(HR) AS homeRuns
FROM batting
WHERE yearId = 2004
GROUP BY teamID
ORDER BY homeRuns DESC
```

```sql
-- 2004년도에 200홈런 이상을 기록한 팀
SELECT TOP 1 teamID, SUM(HR) AS homeRuns
FROM batting
WHERE yearId = 2004 AND homeRuns >= 200 -- Error : homeRuns가 생성되기전 WHERE가 호출됨
GROUP BY teamID
ORDER BY homeRuns DESC

--

SELECT TOP 1 teamID, SUM(HR) AS homeRuns
FROM batting
WHERE yearId = 2004
GROUP BY teamID
HAVING SUM(HR) >= 200 -- 그룹지정 이후 WHERE과 동일
ORDER BY homeRuns DESC

/*
실행순서를 다시 정리하면
FROM
WHERE
GROUP BY
HAVING
SELECT
ORDER BY
*/
```

```sql
-- 단일 년도에 가장 많은 홈런을 날린 팀은?
SELECT teamID, yearID SUM(HR) AS homeRuns
FROM batting
GROUP BY teamID, yearID
ORDER BY homeRuns DESC
```