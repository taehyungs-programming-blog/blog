---
layout: default
title: "02. DATA 다뤄보기"
parent: "(MySQL 💾)"
grand_parent: "Python"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## 사전사항

```sql
-- 테이블 준비
USE dave;
DROP TABLE IF EXISTS mytable;
CREATE TABLE mytable (
    id INT UNSIGNED NOT NULL AUTO_INCREMENT,
    name VARCHAR(20) NOT NULL,
    model_num VARCHAR(10) NOT NULL,
    model_type VARCHAR(10) NOT NULL,
    PRIMARY KEY(id)
);
DESC mytable;
```

---

## CREATE

```sql
INSERT INTO mytable (name, model_num, model_type) VALUES('i7', '7700', 'Kaby Lake');
INSERT INTO mytable (name, model_num, model_type) VALUES('i7', '7500', 'Kaby Lake');    
INSERT INTO mytable (name, model_num, model_type) VALUES('i7', '7700K', 'Kaby Lake');  
INSERT INTO mytable (name, model_num, model_type) VALUES('i7', 'G4600', 'Kaby Lake');  
INSERT INTO mytable (name, model_num, model_type) VALUES('i7', '7600', 'Kaby Lake');  

SELECT * FROM mytable;
```

---

## READ

```sql
-- 연습문제1: model_num 이 7700 으로 시작하는 로우(Row) 검색하기
-- 연습문제2: name 이 i7 인 로우(Row) 검색하기
-- 연습문제3: model_type 이 카비레이크 인 로우(Row) 를 1개만 검색하기(LIMIT 사용)

SELECT * FROM mytable WHERE model_num LIKE '7700%'
SELECT * FROM mytable WHERE name LIKE '%i7%'
SELECT * FROM mytable WHERE model_type LIKE '%카바레이크%' LIMIT 1
```

---

## UPDATE

```sql
UPDATE mytable SET name = 'i5', model_num = '5500' WHERE id = 3;
SELECT * FROM mytable;
```

---

## DELETE

```sql
DELETE FROM mytable WHERE id = 3;
SELECT * FROM mytable;
```

---

## DCL(Data Control Language)

* DCL : mysql 사용자 확인/추가/비빌번호관리

```sql
-- MySQL Command Line Client에서 실행.

-- 1. mysql 사용자 확인
use mysql;  
    -- mysql데이터 베이스 안에는 mysql를 관리하는 데이터가 존재
select * from user;
    -- 현재 사용자 확인

-- 2. 사용자 추가
use mysql;

-- 2-1) 로컬에서만 접속 가능한 userid 생성
create user 'userid'@localhost identified by '비밀번호';

-- 2-2) 모든 호스트에서 접속 가능한 userid 생성
create user 'userid'@'%' identified by '비밀번호';

-- 3. 사용자 비밀번호 변경
mysql> SET PASSWORD FOR 'userid'@'%' = '신규비밀번호';

-- 4. 사용자 삭제
use mysql;
drop user 'userid'@'%';
```



