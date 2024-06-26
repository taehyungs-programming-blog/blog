---
layout: default
title: "19. [문법] 변수와 흐름 제어"
parent: "(DB 연결 기초)"
grand_parent: "(GameServer C# 🎯)"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## 변수 선언

```sql
DECLARE @i AS INT = 10 
-- @는 필수는 아지만 table과 이름이 겹칠까봐 넣음
-- 초기값은 필수는 아니다
DECLARE @i2 AS INT
SET @i2 = 10

-- 예제 역대 최대 연봉을 받은 선수 이름
DECLARE @firstName AS NVARCHAR(15)
DECLARE @lastName AS NVARCHAR(15)
SET @firstName 1 = (SELECT TOP 1 nameFirst
    FROM players AS p
        INNER JOIN slaries AS s
        ON p.playerID = s.playerID
    ORDER BY s.salary DESC);

SELECT @firstName
```

```sql
-- 아래는 MS-SQL만 지원한다
SET TOP 1 @firstName = p.nameFirst, @lastName = p.nameLast
FROM players AS p
    INNER JOIN slaries AS s
    ON p.playerID = s.playerID
ORDER BY s.salary DESC;
```

## 배치(GO)

```sql
DECLARE @firstName AS NVARCHAR(15)
--- ...
DECLARE @firstName AS NVARCHAR(15)  -- Error : 중복 변수 선언
-- 영역을 나누는 방법이 없나? -> 배치
```

```sql
GO
DECLARE @firstName AS NVARCHAR(15)

--- ...

GO
DECLARE @firstName AS NVARCHAR(15) -- Okay
```

* 배치는 아래와 같은 상황에도 사용한다.

```sql
SELECT *
FOM players     -- FROM의 오타, 아래 라인도 모두 출력이 안된다.

SELECT *
FROM salaries   -- 한 줄의 오타로 아래 라인이 모두 동작이 안됨 -> 배치로 구분하자
```

```sql
SELECT *
FOM players     -- FROM의 오타

GO

SELECT *
FROM salaries   -- 정상동작
```

---

## 흐름제어

```sql
-- IF
GO
DECLARE @i AS INT = 10

IF @i = 10
BEGIN
    PRINT('BINGTO!')
END
ELSE
BEGIN
    PRINT('NO!')
END
```

```sql
-- while
DECLARE @i AS INT = 10
WHILE @i <= 10
BEGIN
    PRINT @i
    SET @i = @i + 1
    IF @i = 6 BREAK -- OR CONTINUE
END
```

---

## 테이블 변수

```sql
DECLARE @test TABLE
(
    name VARCHAR(50) NOT NULL
    salary INT NOT NULL
);

INSERT INTO @test
SELECT p.nameFirst + ' ' + p.nameLast, s.salary
FROM players AS p
    INNER JOIN salaries AS s
    ON p.playerID = s.playerID

SELECT *
FROM @test
```