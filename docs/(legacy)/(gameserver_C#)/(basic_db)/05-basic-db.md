---
layout: default
title: "05. 수치와 문자열"
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

## 연산해보기

```sql
USE BaseballData

-- 한국 나이 계산
-- Ex) 2021 - 1934 = ???

SELECT 2021 - 1934
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/database/basic-5-1.png"/>
</p>

```sql
USE BaseballData
SELECT 2021 - birthYear AS koreanAge
FROM players
WHERE deathYear IS NULL AND birthYear IS NOT NULL
ORDER BY koreanAge
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/database/basic-5-2.png"/>
</p>

* 너무 나이가 크면 제외시켜보자

```sql
USE BaseballData
SELECT 2021 - birthYear AS koreanAge
FROM players
WHERE deathYear IS NULL AND birthYear IS NOT NULL AND koreanAge <= 80   -- Error : koreanAge를 못찾음
ORDER BY koreanAge
```

```sql
USE BaseballData
SELECT 2021 - birthYear AS koreanAge
FROM players
WHERE deathYear IS NULL AND birthYear IS NOT NULL AND (2021 - birthYear) <= 80
ORDER BY koreanAge
```

```sql
-- 참고) NULL을 연산하면??
SELECT 2021 - NULL  -- 결과는 NULL로 나온다

SELECT 3 / 2        -- 결과는 1로 나온다 (정수 / 정수 = 정수)
-- 소수를 출력하고 싶다면 3.0 / 2.0 으로 할 것

SELECT 3 / 0        -- Error
```

```sql
-- 기타 지원 함수
SELECT ROUND(3.1443647, 3)      -- 반올림
SELECT POWER(2, 3)              -- 제곱
SELECT COS()                    -- 코사인
```

---

## 문자열 사용

```sql
SELECT N'안녕하세요'    -- N : 유니코드로 표현해주세요
```

```sql
SELECT 'Hello' + 'World'    -- 문자열 결함은 +

-- 문자열 분리
SELECT SUBSTRING('20200425', 1/*시작문자(1부터시작)*/, 4/*4개뽑아달라*/)

-- 빈문자열을 빼달라
SELECT TRIM('       hello')     -- 앞에 공백 다 빠짐
```

```sql
SELECT nameFirst + ' ' + nameLast AS fullName
FROM players
WHERE nameFirst IS NOT NULL AND nameLast IS NOT NULL
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/database/basic-5-3.png"/>
</p>