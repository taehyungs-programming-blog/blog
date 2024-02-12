---
layout: default
title: "01. MySQL 환경설정 / SQL-Query로 TABLE 다뤄보기"
parent: "(MySQL 💾)"
grand_parent: "Python"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## 설치

* [Link MySQL WorkBench 🌎](https://www.mysql.com/downloads/) : 아래의 MySQL Community Down
* [Link MySQL Server 🌎](https://dev.mysql.com/downloads/mysql/) : MySQL Sever 다운

---

## 이미 MySQL이 설치되어 있는데 비밀번호를 까먹은 경우?

* 특별한 경우가 아니라면, 그냥 MySQL 서버를 재설치하자
  * MySQL Server XX 삭제 후 재설치하면 됨.

---

## MySQL 설치확인

* 서비스에서 설치 후 동작중인지 확인이 가능하다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/python/mysql/mysql-1-1.png"/>
</p>

* 명령어 넣어보기 -> **MySQL Workbench** 실행
  * MySQL Connections에서 Local instance연결

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/python/mysql/mysql-1-2.png"/>
</p>

---

## DB 만들기

```sql
-- 생성
CREATE DATABASE dbname;

-- 확인
SHOW DATABASES;

-- 삭제
DROP DATABASE IF EXISTS dbname;

-- 사용선언
USE dbname;
```

---

## TABLE 만들기

```sql
CREATE DATABASE dave;
USE dave;

-- 예를들어 아래와 같은 TABLE을 만들고자 한다
CREATE TABLE myproduct (
    KEY INT,
    ID STRING,
    TITLE STRING,
    ORI_PRICE INT,
    DISCOUNT_PRICE INT,
    DISCOUNT_PERCENT INT,
    DELEVERY STRING,
    PRIMARY_KEY KEY(KEY)
);
-- 어떻게 하면 좋을까?
```

* 우선, 데이터형은 매우 다양하게 있는데 자주사용되는 것을 나열하자면
    * `INT`
    * `FLOAT`, `DOUBLE`
    * `TEXT`
    * `DATETIME`

```sql
USE dave;

CREATE TABLE mytable (
    id INT UNSIGNED NOT NULL AUTO_INCREMENT,
    name CHAR(20) NOT NULL,
    age TINYINT,
    phone VARCHAR(20),
    email VARCHAR(30) NOT NULL,
    address VARCHAR(50),
    PRIMARY KEY(id)
);

-- 확인
SHOW TABLES;

-- 삭제
DROP TABLE mytable;
```

```sql
-- 정리하자면
USE dave;
DROP TABLE IF EXISTS mytable;
CREATE TABLE mytable (
    id INT UNSIGNED NOT NULL AUTO_INCREMENT,
    name CHAR(20) NOT NULL,
    age TINYINT,
    phone VARCHAR(20),
    email VARCHAR(30) NOT NULL,
    address VARCHAR(50),
    PRIMARY KEY(id)
);
SHOW TABLES;
DESC mytable;
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/python/mysql/mysql-1-3.png"/>
</p>


---

## TABLE 변경

🍁 그냥 예제로 보여주는게 가장 쉬움.

```sql
USE dave;
DROP TABLE IF EXISTS mytable;
CREATE TABLE mytable (
    id INT UNSIGNED NOT NULL AUTO_INCREMENT,
    name CHAR(20) NOT NULL,
    age TINYINT,
    phone VARCHAR(20),
    email VARCHAR(30) NOT NULL,
    address VARCHAR(50),
    PRIMARY KEY(id)
);
DESC mytable;

ALTER TABLE mytable MODIFY COLUMN name varchar(20) NOT NULL;
DESC mytable;
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/python/mysql/mysql-1-4.png"/>
</p>

```sql
ALTER TABLE mytable CHANGE COLUMN phone model_num varchar(10) NOT NULL;
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/python/mysql/mysql-1-5.png"/>
</p>

```sql
-- 삭제
ALTER TABLE mytable DROP COLUMN age;
```
