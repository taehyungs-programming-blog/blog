---
layout: default
title: "26. [튜닝] 인덱스 컬럼 순서"
parent: "(DB 연결 기초)"
grand_parent: "(GameServer C# 🎯)"
nav_order: 3
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## 복합 인덱스의 컬럼 순서

* 복합 인덱스를 걸 경우 **인덱스의 순서에 따라 성능**에 영향을 미치게 된다.

```sql
-- 데이터 생성
USE Northwind;

SELECT *
INTO TestOrders
FROM Orders;

DECLARE @i INT = 1;
DECLARE @emp INT;
SELECT @emp = MAX(EmployeeID) FROM Orders;

WHILE (@i < 1000)
BEGIN
    INSERT INTO TestOrders(CustomerID, EmployeeID, OrderDate)
    SELECT CustomerID, @emp + @i, OrderDate
    FROM Orders;
    SET @i = @i + 1;
END
GO

-- 데이터가 몇개인지 보자
SELECT COUNT(*)
FROM TestOrders;
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/database/basic-26-1.png"/>
</p>

```sql
-- 두 개의 INDEX를 순서만 바꿔서 걸어보자.
CREATE NONCLUSTERED INDEX idx_emp_ord
ON TestOrders(EmployeeID, OrderDate);

CREATE NONCLUSTERED INDEX idx_ord_emp
ON TestOrders(OrderDate, EmployeeID);
```

```sql
-- 속도 체크를 위한 옵션 ON
SET STATISTICS TIME ON;
SET STATISTICS IO ON;

SELECT *
FROM TestOrders WITH(INDEX(idx_emp_ord))
WHERE EmployeeID = 1 AND OrderDate = '19970101';

SELECT *
FROM TestOrders WITH(INDEX(idx_ord_emp))
WHERE EmployeeID = 1 AND OrderDate = '19970101';
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/database/basic-26-2.png"/>
</p>

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/database/basic-26-3.png"/>
</p>

* 잉??? 성능에 차이가 없는데???

```sql
SELECT *
FROM TestOrders
ORDER BY EmployeeID, OrderDate;
```

* EmployeeID, OrderDate 순으로 정렬되어 있음을 볼 수 있다.
* sql입장에서 `WHERE EmployeeID = 1 AND OrderDate = '19970101';` 대로 검색해 보자면 `EmployeeID = 1`인 리프로 이동후 순차적으로 `OrderDate = '19970101'`을 찾게 된다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/database/basic-26-4.png"/>
</p>

```sql
SELECT *
FROM TestOrders
ORDER BY OrderDate, EmployeeID;
```

* OrderDate, EmployeeID 순으로 정렬되어 있음을 볼 수 있다.
* sql입장에서  `WHERE EmployeeID = 1 AND OrderDate = '19970101';` 대로 검색해 보자면 `OrderDate = '19970101'`인 리프로 이동후 순차적으로 `EmployeeID = 1`를 찾게 된다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/database/basic-26-5.png"/>
</p>

* 말하고자 하는 것은 두 INDEX 순서에 따라 검색의 절차가 **크게 차이가 없다는 것**이다.

---

* 이번엔 **범위로 찾아보자.**

```sql
SELECT *
FROM TestOrders WITH(INDEX(idx_emp_ord))
-- WHERE EmployeeID = 1 AND OrderDate >= '19970101' AND OrderDate <= '19970103';
WHERE EmployeeID = 1 AND OrderDate BETWEEN '19970101' AND '19970103';

SELECT *
FROM TestOrders WITH(INDEX(idx_ord_emp))
WHERE EmployeeID = 1 AND OrderDate BETWEEN '19970101' AND '19970103';
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/database/basic-26-6.png"/>
</p>

* 성능의 차이가 존재한다. 👉 순서가 EmployeeID, OrderDate인 Index는 `EmployeeID = 1`를 찾고 OrderDate도 정렬이 되어 있기에 빠른 반면
* 순서가 OrderDate, EmployeeID인 Index는  OrderDate는 정렬이 되어 있으나 범위이기에 방대하며, 그 방대한 데이터의 EmployeeID를 순차적으로 다 돌아야 한다.

* **(TIP)** `BETWEEN`의 범위가 작을 경우 👉 IN-LIST로 대체하는 것을 고려하라

```sql
SET STATISTICS PROFILE ON;

SELECT *
FROM TestOrders WITH(INDEX(idx_ord_emp))
WHERE EmployeeID = 1 AND OrderDate IN ('19970101','19970102' ,'19970103');
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/database/basic-26-7.png"/>
</p>

* 속도가 개선이 되는데 `IN`의 경우 `OR`문을 쓴다고 생각하자