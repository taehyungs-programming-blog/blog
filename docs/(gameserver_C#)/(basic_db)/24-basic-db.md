---
layout: default
title: "24. [튜닝] Index Scan vs Index Seek"
parent: "(DB 연결 기초)"
grand_parent: "(GameServer C# 🎯)"
nav_order: 3
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* Index Scan은 **악**이고, Index Seek은 **선**일까?
* 대부분은 그렇지만 아닐 경우도 존재하고 ... 그렇기에 이론적 내용을 알아야 한다.

---

```sql
CREATE TABLE TestAccess
(
    id INT NOT NULL,
    name NCHAR(50) NOT NULL,
    dummy NCHAR(1000) NULL
);
GO


-- index를 걸어주자
CREATE CLUSTERED INDEX TestAccess_CI
ON TestAccess(id);
GO

CREATE NONCLUSTERED INDEX TestAccess_NCI
ON TestAccess(name);
GO

-- 인덱스 정보 확인
EXEC sp_helpindex 'TestAccess';

-- 인덱스 번호 확인
SELECT index_id, name
FROM sys.indexes
WHERE object_id = object_id('TestAccess');

-- 조회
DBCC IND('Northwind', 'TestAccess', 1);
DBCC IND('Northwind', 'TestAccess', 2);
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/database/basic-24-1.png"/>
</p>

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/database/basic-24-2.png"/>
</p>

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/database/basic-24-3.png"/>
</p>

```sql
DECLARE @i INT;
SET @i = 1;

WHILE (@i <= 500)
BEGIN
    INSERT INTO TestAccess
    VALUES (@i, 'Name' + CONVERT(VARCHAR, @i), 'Hello World ' + CONVERT(VARCHAR, @i));
    SET @i = @i + 1;
END
```

```sql
SELECT *
FROM TestAccess;
-- F5 -> Ctrl + L
```

* Index Scan을 기반으로 동작

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/database/basic-24-4.png"/>
</p>

* 좀 더 많은 정보를 보기 위해서

```sql
SET STATISTICS TIME ON;
SET STATISTICS IO ON;

SELECT *
FROM TestAccess;
```

* 논리적 읽기 숫자가 169임을 볼 수 있는데, 현재 Leaf PAGE의 개수가 168개라 모두 읽는다고 보면된다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/database/basic-24-5.png"/>
</p>

```sql
SET STATISTICS TIME ON;
SET STATISTICS IO ON;

SELECT *
FROM TestAccess
WHERE id = 104; -- CLUSTERED
```

* 경과 시간이 거의 0에 근접하며, 논리적 읽기가 2회이다.
* ROOT에서 id값을 검색후 바로 PAGE로 이동하기에 논리적 읽기가 2회이며 속도가 빠르다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/database/basic-24-6.png"/>
</p>

```sql
SET STATISTICS TIME ON;
SET STATISTICS IO ON;

SELECT *
FROM TestAccess
WHERE name = 'name5'; -- NON-CLUSTERED
```

* 논리적 읽기 4회 👉 Non-Clustered Table에가서 name을 탐색(2회) Non-Clustered Table은 Clustered Table Id만을 갖기에 다시 Clustered Table에서 탐색 (2회) = 총 4회
* NON-CLUSTERED의 경우 CLUSTERED의 id값만 갖고 있기에 검색후 CLUSTERED id를 기반으로 데이터를 찾아간다.
* 따라서 논리적 읽기 횟수가 더 늘어남.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/database/basic-24-7.png"/>
</p>


```sql
SET STATISTICS TIME ON;
SET STATISTICS IO ON;

SELECT TOP 5 *
FROM TestAccess
ORDER BY name;
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/database/basic-24-8.png"/>
</p>

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/database/basic-24-9.png"/>
</p>

* 인덱스 스캔을 하는데도 논리적 읽기나 속도가 나쁘지 않다?
* ORDER BY name을 하는데 name이 NON-CLUSTER이기에 TOP 5를 읽을 시 순차적으로 읽으면 됨 👉 성능에 영향을 미치지 않는 요인!