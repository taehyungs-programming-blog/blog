---
layout: default
title: "04. ORDER BY"
parent: "(DB 연결 기초)"
grand_parent: "(GameServer C# 🎯)"
great_grand_parent: "Legacy Archive"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## 정렬과 연산

```sql
USE BaseballData	-- BaseballData DB를 써주세요

SELECT *
FROM players
WHERE birthYear IS NOT NULL
ORDER BY birthYear
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/database/basic-4-1.png"/>
</p>

```sql
USE BaseballData

SELECT *
FROM players
WHERE birthYear IS NOT NULL
ORDER BY birthYear DESC -- 역순정렬
```

* birthYear만 아니라 Month, Day까지 적용해 보자.

```sql
USE BaseballData

SELECT *
FROM players
WHERE birthYear IS NOT NULL
ORDER BY birthYear DESC, birthMonth DESC, birthDay DESC
```

---

## 필터 넣기

```sql
USE BaseballData

SELECT TOP 10 *     -- 상위 10명만 보여달라(참고로 TOP은 MS-SQL 기능임)
FROM players
WHERE birthYear IS NOT NULL
ORDER BY birthYear DESC, birthMonth DESC, birthDay DESC
```

```sql
USE BaseballData

SELECT TOP 1 PERCENT *     -- 상위 1%만 보여달라
FROM players
WHERE birthYear IS NOT NULL
ORDER BY birthYear DESC, birthMonth DESC, birthDay DESC
```

```sql
USE BaseballData

SELECT *
FROM players
WHERE birthYear IS NOT NULL
ORDER BY birthYear DESC, birthMonth DESC, birthDay DESC
OFFSET 100 ROWS FETCH NEXT 100 ROWS ONLY
-- 100~200 을 보여달라
```