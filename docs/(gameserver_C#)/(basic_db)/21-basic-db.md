---
layout: default
title: "21. [튜닝] 인덱스 분석"
parent: "(DB 연결 기초)"
grand_parent: "(GameServer C# 🎯)"
nav_order: 3
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* 지금부터는 문법이라기 보단 **성능향상**을 위한 시도이다.(sql 튜닝이다)
* [instnwnd.sql](https://raw.githubusercontent.com/microsoft/sql-server-samples/master/samples/databases/northwind-pubs/instnwnd.sql)를 다운 혹은 소스를 복사하여 실행하면 자동으로 db가 생성된다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/database/basic-21-1.png"/>
</p>

```sql
USE Northwind;

-- DB 정보를 보여주세요
EXEC sp_helpdb 'Northwind';
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/database/basic-21-2.png"/>
</p>

* 본 강의서 하고자 하는 것은 **인덱스**를 직접 눈으로 확인하는 것이다.

```sql
-- 임시 테이블 생성(인덱스 테스트용)
CREATE TABLE Test
(
	EmployeeID INT NOT NULL,
	LastName NVARCHAR(20) NULL,
	FirstName NVARCHAR(20) NULL,
	HireDate DATETIME NULL
);
GO

-- dbo.Employees에서 EmployeeID, LastName, FirstName, HireDate를 Test dbo로 넣어달라
INSERT INTO Test
SELECT EmployeeID, LastName, FirstName, HireDate
FROM Employees;

-- 잘 들어갔나 확인용
SELECT *
FROM Test;
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/database/basic-21-3.png"/>
</p>

* 아래 내용을 정확히 아직은 **이해할 수 없다**
* **비효율적으로 index를 할당**했다고 받아들이자

```sql
-- Index 넣기 : 
	-- FILLFACTOR - 리프 페이지 공간의 1%만 사용(일단은 받아들이자)
        -- 테이블을 강제로 트리구조로 만들기 위해서이다.
	-- PAD_INDEX - FILLFACTOR 중간 페이지 적용
CREATE INDEX Test_Index ON Test(LastName)
WITH (FILLFACTOR = 1, PAD_INDEX = ON)
GO
```

```sql
-- 인덱스 번호 찾기
  -- 인덱스 정보를 위해서 필요함
SELECT index_id, name
FROM sys.indexes
WHERE object_id = object_id('Test');
```

* 인덱스 번호가 **2**이다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/database/basic-21-4.png"/>
</p>

* 자! 여기까지 한걸 **정리**해 보자
    * `CREATE INDEX Test_Index ON Test(LastName)` - Test Table의 Index를 LastName으로 지정
    * 단, `WITH (FILLFACTOR = 1, PAD_INDEX = ON)` 옵션을 넣어 아주 나쁜 성능의 Index를 넣음
    * 그리고 넣은 Index의 Id는 **2**이다.
* 일단은 이렇게만 이해해도 좋다.

```sql
-- 2번(Test) 인덱스 정보 살피기
  -- Northwind DB의 Test Table의 Index번호가 2인 인덱스정보를 알려주세요
DBCC IND('Northwind', 'Test', 2);
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/database/basic-21-5.png"/>
</p>

```sql
/*
 * IndexLevel은 아래와 같은 구조이다. *

        Root(2)
           |
        Branch(1)
           |
        Leaf(0)
*/
```

* 최상위 IndexLevel이 2임을 기억하고 아래를 보자

```sql
/*
우리의 예시로 다시 그려보자면 ...
            849
            / \
          872 848
          / \ / \
       832  840  841
*/

-- 위와 같은 트리구조로 데이터를 찾아감을 알 수 있다
```

```sql
DBCC PAGE('Northwind', 1/*파일번호*/, 849/*페이지번호*/, 3/*출력옵션*/);
-- HEAP RID([페이지 주소(4)][파일ID(2)][슬롯번호(2)]를 조합한 ROW 식별자.
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/database/basic-21-6.png"/>
</p>

```sql
DBCC PAGE('Northwind', 1/*파일번호*/, 840/*페이지번호*/, 3/*출력옵션*/);
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/database/basic-21-7.png"/>
</p>

* 832, 840, 841에 데이터가 들어가있음을 확인할 수 있다.
* 데이터를 탐색할 시 `Table [ Page | Page | Page ... ]` Table내의 Page를 탐색해 데이터를 찾는다.

---

## (핵심) 그래서 SQL은 어떻게 데이터를 찾을까?

* 우선 전체 데이터는 아래와 같다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/database/basic-21-8.png"/>
</p>

* 위에서 우리는 LastName에 Index를 (비효율적으로)걸었고 그에 따라 Page가 할당됐을것이다.

```sql
DBCC PAGE('Northwind', 1, 849, 3);
DBCC PAGE('Northwind', 1, 848, 3);
DBCC PAGE('Northwind', 1, 872, 3);
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/database/basic-21-9.png"/>
</p>

```sql
DBCC PAGE('Northwind', 1, 832, 3);
DBCC PAGE('Northwind', 1, 840, 3);
DBCC PAGE('Northwind', 1, 841, 3);
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/database/basic-21-10.png"/>
</p>

* 대략 tree 구조로 살펴보자면

```sql
/*
                     849(Leverling ~)
                     /             \
          848(Dodsworth ~)   872(Leverling ~)
                 /      \       /           \
       832(Buchanan ~)  840(Dodsworth ~)  841(Leverling ~)
*/
```

* 예를들어 Callahan을 찾는다면 849에서 L보다 C가 앞이니 848로
* Dogsworth의 D보다 C가 앞이니 832로 832가 마지막 level이니 832에서 탐색하는 구조이다.

* 용어 정리
  * **Random Access** - 데이터를 찾기 위해 Page하나하나를 이동
  * **Bookmark Lookup** - 찾은 Page에서 데이터를 찾음