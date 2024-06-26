---
layout: default
title: "06. DATETIME"
parent: "(DB 연결 기초)"
grand_parent: "(GameServer C# 🎯)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## 날짜를 관리하는 자료형

```sql
-- DATE 연/월/일
-- TIME 시/분/초
-- DATETIME 연/월/일/시/분/초
```

* 문자열을 DATETIME로 캐스팅이 가능하다

```sql
SELECT CAST('20200425' AS DATETIME)
-- YYYYMMDD 포맷임 참고
-- YYYYMMDD hh:mm:ss.nnn
-- YYYY-MM-DD
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/database/basic-6-1.png"/>
</p>

```sql
-- 현재시간 받기
SELECT GETDATE()            -- T-SQL 표준
SELECT CURRENT_TIMESTAMP    -- SQL 표준

-- 단 여기서 받는 시간은 서버 PC의 시간이 기준이 된다.
-- 만약 세계에 서비스하는 경우라면???
SELECT GETUTCDATE()         -- UTC 시간 받음
```

```sql
SELECT *
FROM datetimeTest
WHERE time >= '20100101'
```

* 시간에 연산(몇분 더하기, 유효기간 계산 등)

```sql
SELECT DATEADD(YEAR, 1, '20200426')     -- 20200426에서 1년 더하기
SELECT DATEADD(DAY, 5, '20200426')     -- 20200426에서 5일 더하기

SELECT DATEADD(DAY, -5, '20200426')     -- 20200426에서 5일 빼기
```

* 두 시간간의 차이

```sql
SELECT DATEDIFF(SECOND, '20200426', '20200503')
```

```sql
-- 년/월/일 뽑기
SELECT DATEPART(DAY, '20200826')
SELECT DAY('20200826')              -- 이거도 가능
```