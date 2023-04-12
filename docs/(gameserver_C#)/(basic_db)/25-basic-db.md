---
layout: default
title: "25. [튜닝] 북마크 룩업"
parent: "(DB 연결 기초)"
grand_parent: "(GameServer C# 🎯)"
nav_order: 3
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## 잠깐 복습 및 이론

* Non-Clustered, Clustered는 구조는 아래와 같이 동일하며

```sql
/*
Non-Clustered
      1
2 3 4 5 6 7 8
<실제 데이터는 Heap Table OR 
Clustered가 있다면 Clustered에 존재>

Clustered
      1
2 3 4 5 6 7 8 
<실제로 데이터가 Leaf에 존재>
*/
```

* 차이점 이라고 한다면 Non-Clustered경우 Clustered가 없을경우 Heap Table에 데이터를 보관하고
* Clustered같은 경우 직접 데이터를 갖고 있다. (따라서 Index Seek로 동작하며 빠르다, 찾아간 Leaf에 바로 데이터가 있으니)
* Non-Clustered의 경우 데이터가 Leaf PAGE에 없고 한 번더 찾아가야하는데 **RID(Heap Table) 혹은 Key(Clustered)**로 찾아야 한다
* 여기서 RID(Heap Table) 방식(ID를 기반 데이터를 찾아감)이 **북마크 룩업**이다. 

* Index를 써도 어떤경우(대부분 Index Scan)는 **속도가 나빠질 수** 있는데, 왜 그런 일이 생길까?
* 이유는 데이터를 찾는 방식인 **북마크 룩업**에 있다. 

---

## 실습

```sql
-- 데이터 생성
SELECT *
INTO TestOrders
FROM Orders;

SELECT *
FROM TestOrders;
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/database/basic-25-1.png"/>
</p>

```sql
-- Index 걸기
CREATE NONCLUSTERED INDEX Orders_Index01
ON TestOrders(CustomerID);

-- Index 확인
SELECT index_id, name
FROM sys.indexes
WHERE object_id = object_id('TestOrders');
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/database/basic-25-2.png"/>
</p>

```sql
DBCC IND('Northwind', 'TestOrders', 2);
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/database/basic-25-3.png"/>
</p>

```sql
/*
     8952
8912 8920 8921

* Clustered를 만들지 않았기에 Heap Table이 존재할 것이다. *
*/
```

```sql
-- 테스트를 위해 옵션을 몇개 켜주자
SET STATISTICS TIME ON; -- 실행시간
SET STATISTICS IO ON;
SET STATISTICS PROFILE ON; -- 실행순서
```

* `PROFILE`의 결과는 아래와 같이 나온다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/database/basic-25-8.png"/>
</p>

```sql
SELECT *
FROM TestOrders
WHERE CustomerID = 'QUICK';
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/database/basic-25-4.png"/>
</p>

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/database/basic-25-5.png"/>
</p>


```sql
SELECT *
FROM TestOrders WITH(INDEX(Orders_Index01)) -- index를 강제로 사용하게 해보자
WHERE CustomerID = 'QUICK';
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/database/basic-25-6.png"/>
</p>

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/database/basic-25-7.png"/>
</p>

* 그런데 **룩업북**이 느린이유를 아직 모르겠는데?? 사실 위 처럼 index를 강제로 지정할 경우 논리적 검색도 빠른거 같은데?? 
    * 👉 맞다! 하지만 Heap-Table이 문제가 된다. Heap-Table은 실 데이터에 접근이 아니라 메모리접근인데 
    * 만약 Heap-Table이 메모리에 올라가 있지 않는 경우 디스크에서 메모리로 읽어와 다시 메모리를 읽게된다. 이 과정에서 속도의 차이가 발생하게 된다.

```sql
-- 룩업을 줄여보자
SELECT *
FROM TestOrders WITH(INDEX(Orders_Index01))
WHERE CustomerID = 'QUICK' AND ShipVia = 3;
```

* 데이터는 8개를 찾고 PROFILE을 볼 경우 28번을 실행해서 8개를 찾음
* Non-Clustered는 CustomerID에 연결되어 있고 Non-Clustered의 Leaf에 들어가 ShipVia를 찾기에 이렇게 실패확률이 높아진다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/database/basic-25-9.png"/>
</p>


```sql
-- 기존의 Index를 제거하고
DROP INDEX TestOrders.Orders_Index01;

-- 새로운 Index를 생성해 보자
CREATE NONCLUSTERED INDEX Orders_Index01
ON TestOrders(CustomerID, ShipVia);
```

```sql
SELECT *
FROM TestOrders WITH(INDEX(Orders_Index01))
WHERE CustomerID = 'QUICK' AND ShipVia = 3;
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/database/basic-25-10.png"/>
</p>

* 이렇게 Index항목을 늘리는 방식을 **Covered Index**라 한다.
* 그럼 **복합 인덱스는 무조건 성능에 유리**한가?? 👉 역시 꼭 그렇지는 않다(Insert, Delete, Update)가 느려진다. **(정렬이 필요하기 때문에)**
* **결론** : 데이터 수정이 없는 경우 Covered Index가 유리, 다른경우 필요한 경우만 Index를 달라

```sql
-- 좀 더 진화된 버전을 만들어 보자.
DROP INDEX TestOrders.Orders_Index01;

-- 데이터 정렬은 CustomerID로 하고 Leaf Page에 ShipVia를 넣겠다.
-- 따라서 Heap Table을 메모리에 직접 올리지 않아도 된다.
CREATE NONCLUSTERED INDEX Orders_Index01
ON TestOrders(CustomerID) INCLUDE (ShipVia);
```

```sql
SELECT *
FROM TestOrders WITH(INDEX(Orders_Index01))
WHERE CustomerID = 'QUICK' AND ShipVia = 3;
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/database/basic-25-11.png"/>
</p>

* 이렇게 해도 답이 없다면 **Clustered-Index**를 활용하자