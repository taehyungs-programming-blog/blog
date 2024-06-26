---
layout: default
title: "02. [문법] SQL이란? + Table만들어 보기"
parent: "(DB 연결 기초)"
grand_parent: "(GameServer C# 🎯)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## SQL 이란?

* RDBMS를 조작하기 위한 언어(명령어)
* MS-SQL은 T-SQL이라고 부르기도한다.(SQL에서 몇가지 기능이 추가 되었기에)
* 단, 표준 SQL을 쓰는 것을 추천...
* CRUD(Create / Read / Update / Delete)를 주로 다룬다.

---

## 쿼리 날려보기

* `ctrl + n` : **파일 -> 새로 만들기 -> 현재 연결로 쿼리**

```sql
SELECT *
FROM dbo.players;
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/database/basic-2-1.png"/>
</p>

---

## 주석달기

```sql
/* 주석 */
-- 주석
```

---

## 데이터 베이스의 구성

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/database/basic-2-2.png"/>
</p>

* 위 그림과 같이 

```
데이터 베이스 ---- 테이블
               |
               --- 테이블
```

* 이러한 구조이다.
* 마치 엑셀파일의 **엑셀파일 안에 시트**가 여러개 들어가 있는 구조와 동일하다 생각하면 된다.

---

## 테이블을 생성 후 데이터를 몇개 넣어보자

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/database/basic-2-3.png"/>
</p>

* 테이블 우클릭 -> 새로 만들기 -> 테이블
* 열에 name, english, math, korean (`nvarchar(50)` : n(Unicode) + varchar(String) + 50(50자)) 추가 -> ExamResult로 저장

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/database/basic-2-6.png"/>
</p>

* dbo.ExamResult 우 클릭 후 **상위 1000개 행 선택** 클릭

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/database/basic-2-4.png"/>
</p>

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/database/basic-2-5.png"/>
</p>