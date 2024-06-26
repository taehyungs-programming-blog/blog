---
layout: default
title: "28. [튜닝] Merge 조인"
parent: "(DB 연결 기초)"
grand_parent: "(GameServer C# 🎯)"
nav_order: 3
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* **Merge Join**은 **Sort Merge Join(정렬 병합 조인)**이라고 불리기도한다.

```sql
USE BaseballData;

SELECT *
FROM players AS p
INNER JOIN salaries AS s
ON p.playerID = s.playerID;
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/database/basic-28-1.png"/>
</p>

* **Merge Join**은 Key를 기반으로 (OUTTER, INNER 모두) 우선 한 번 **Sort**한다.
* 이후 테이블의 Key값을 비교/순회 하며 데이터를 추가한다.

* Merge Join역시 조건이 붙는데, **OUTTER가 UNIQUE**해야한다.(Primary key이거나 Unique이거나)
* 필수 조건은 아니고 속도가 훨씬 빨라진다. 이유는 OUTTER Table key를 INNER Table 에서 탐색하는데 INNER Table에도 해당 key가 없다면 이후 검색할 INNER Table 의 갯수가 줄어들기 때문
* **Clustered Scan 후 정렬**하면 빨라진다.

```sql
SELECT *
FROM schools AS s
INNER JOIN schoolsplayers AS p
ON p.schoolID = s.schoolID;
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/database/basic-28-2.png"/>
</p>

* **결론**
  * OUTTER, INNER를 Sort하고 Merge한다.
  * 이미 정렬된 상태라면 속도가 훨씬 빠르다.
  * 정렬할 데이터가 너무 많을경우 Hash가 오히려 빠르다
  * OUTTER가 Unique하지 않으면 느리다.