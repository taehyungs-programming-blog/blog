---
layout: default
title: "07. CASE (Switch문)"
parent: "(DB 연결 기초)"
grand_parent: "(GameServer C# 🎯)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

```sql
-- 어떤 계절에 태어났는지를 추가하고자 한다.
SELECT *, 
    CASE birthMonth
        WHEN 1 THEN N'겨울'
        WHEN 2 THEN N'겨울'
        WHEN 3 THEN N'봄'
        WHEN 4 THEN N'봄'
        WHEN 5 THEN N'봄'
        WHEN 6 THEN N'여름'
        WHEN 7 THEN N'여름'
        WHEN 8 THEN N'여름'
        WHEN 9 THEN N'가을'
        WHEN 10 THEN N'가을'
        WHEN 11 THEN N'가을'
        WHEN 12 THEN N'겨울'
        ELSE N'알수없음'
    END AS birthSeason
FROM players
```

* 물론 아래처럼 더 깔끔하게 만들수 있다.

```sql
SELECT *, 
    CASE
        WHEN birthMonth <= 2 THEN N'겨울'
        WHEN birthMonth <= 5 THEN N'봄'
        WHEN birthMonth <= 8 THEN N'여름'
        WHEN birthMonth <= 11 THEN N'가을'
        ELSE N'겨울'
    END AS birthSeason
FROM players
```