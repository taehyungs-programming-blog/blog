---
layout: default
title: "23. [튜닝] Clustered vs Non-Clustered"
parent: "(DB 연결 기초)"
grand_parent: "(GameServer C# 🎯)"
nav_order: 3
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## 인덱스 종류에 대해서

* [참고사이트 INDEX에 관해서 🌎](https://taehyungs-programming-blog.github.io/blog/docs/game-server/database/15-db/)

* **Clustered**(영한 사전)
    * Leaf Page에 실제 Data Page(데이터)가 저장이 된다.
    * 참고) Leaf Page는 이전강의에서 Root - Branch - Leaf로 순차적으로 PAGE를 관리했는데 거기서 말하는 Leaf가 맞음
    * 데이터는 Clustered Index 키 순서로 정렬
* **Non-Clustered**(색인)
    * Clustered Index 유무에 따라 다르게 동작됨
    * Clustered Index [유] 👉 Leaf Table에 실제 데이터가 있다, Clustered Index의 키를 토대로 검색
    * Clustered Index [무] 👉 데이터는 Heap Table에 저장, HeapRID가 Heap Table이 된다

---

## 실습

```sql
USE Northwind
SELECT *
INTO TestOrderDetails
FROM [Order Details];

SELECT *
FROM TestOrderDetails;
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/database/basic-23-3.png"/>
</p>

```sql
-- 인덱스 추가
CREATE INDEX Index_OrderDetails
ON TestOrderDetails(OrderID, ProductID);

-- 잘 들어갔나 확인
EXEC sp_helpindex 'TestOrderDetails';
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/database/basic-23-1.png"/>
</p>

```sql
-- 인덱스 번호 찾기
SELECT index_id, name
FROM sys.indexes
WHERE object_id = object_id('TestOrderDetails'); -- 2

DBCC IND('Northwind', 'TestOrderDetails', 2);
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/database/basic-23-2.png"/>
</p>

```sql
/*
             944
              |
856, 888, 889, 890, 891, 982
*/
```

```sql
DBCC PAGE('Northwind', 1, 856, 3);
-- (Non-Clustered이기에) HEAP RID가 존재하며 
-- HEAP RID는 ([페이지 주소(4)][파일ID(2)][슬롯(2)])으로 만들어 진다.

-- 참고)
-- PageType == 1 --> DATA PAGE
-- PageType == 2 --> INDEX PAGE
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/database/basic-23-4.png"/>
</p>

---

* 여기에 **Clustered-Index**를 추가해 보자.

```sql
CREATE CLUSTERED INDEX Index_OrderDetails_Clustered
ON TestOrderDetails(OrderID);

EXEC sp_helpindex 'TestOrderDetails';
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/database/basic-23-5.png"/>
</p>

```sql
DBCC IND('Northwind', 'TestOrderDetails', 2);
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/database/basic-23-6.png"/>
</p>

* 다른 변화는 없지만 **Page ID(PagePID)가 변화**했음을 알 수 있다.

```sql
DBCC PAGE('Northwind', 1, 976, 3);
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/database/basic-23-7.png"/>
</p>

* **HEAP RID도 사라졌음**을 확인할 수 있다.
* 참고로 **UNIQUIFIER**는 Key가 동일한 값을 갖을수 있기에 같은 Key에 들어가는 식별자라고 생각하자