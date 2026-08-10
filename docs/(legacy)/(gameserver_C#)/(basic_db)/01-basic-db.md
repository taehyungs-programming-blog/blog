---
layout: default
title: "01. 환경설정"
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

## Install MS-SQL

* [MS-SQL Download 🌍](https://www.microsoft.com/ko-kr/sql-server/sql-server-downloads) 👉 **Express** 다운

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/database/basic-1-1.png"/>
</p>

* 나머지 설치는 기본으로해도 무방하다 👉 **SSMS** 도 설치

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/database/basic-1-2.png"/>
</p>

* SSMS 설치하는거 잊지말기
    * 잊고 넘겼다면 [SSMS 홈페이지](https://docs.microsoft.com/ko-kr/sql/ssms/download-sql-server-management-studio-ssms?redirectedfrom=MSDN&view=sql-server-ver15)가서 다운받아서 설치해야함

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/database/basic-1-7.png"/>
</p>

---

## 실행

* **Microsoft SQL Server Management Studio** 실행

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/database/basic-1-3.png"/>
</p>

* 셈플데이터를 다운받아보자 [링크 🌍](https://www.sqlskills.com/sql-server-resources/sql-server-demos/)
* **Baseball Stats Sample Database (23MB zip/110MB backup)** 다운

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/database/basic-1-6.png"/>
</p>

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/database/basic-1-4.png"/>
</p>

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/database/basic-1-5.png"/>
</p>

* 데이터베이스 우클릭 -> 데이터 베이스 복원 -> 디바이스 -> 원하는 db선택

* 잘 로드된 것인가 확인해 보자.

```sql
-- ctrl + n
USE BaseballData

SELECT *
FROM dbo.players

-- F5
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/database/basic-1-8.png"/>
</p>