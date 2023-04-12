---
layout: default
title: "18. [문법] TRANSACTION"
parent: "(DB 연결 기초)"
grand_parent: "(GameServer C# 🎯)"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

```sql
USE GameDB

SELECT *
FROM accounts

-- TRAN을 명시하지 않으면 자동 COMMIT
INSERT INTO accounts VALUES(1, 'rookiss', 100, GETUTCDATE())

DELETE accounts

-- 문법
-- BEGIN TRAN or TRANSACTION
-- COMMIT or ROLLBACK

-- 예제) 게임상의 거래를 하는데 A캐릭은 아이템이 옮겨졌는데 B에서 어떤 이유로 옮겨지지 않았다면??

-- 다시 메일로 예를 들면
-- 메일을 쓴다 BEGIN TRAN
-- 메일을 보낸다 COMMIT
-- 보내지 않는다 ROLLBACK

-- 즉, 성공/실패 여부에 따라 COMMIT을 한다

BEGIN TRAN
    INSERT INTO accounts VALUES(1, 'rookiss', 100, GETUTCDATE())
ROLLBACK    -- ROLLBACK을 하면 데이터가 들어가지 않는다
COMMIT      -- COMMIT을 하면 데이터가 들어간다
```

```sql
BEGIN TRY
    BEGIN TRAN
        INSERT INTO accounts VALUES(1, 'rookiss', 100, GETUTCDATE())
        INSERT INTO accounts VALUES(2, 'rookiss2', 100, GETUTCDATE())
    COMMIT
END TRY
BEGIN CATCH
    IF @@TRANCOUNT > 0 -- 현재 활성화된 트랜잭션 수 반환
        ROLLBACK    -- 하나라도 트랜잭션을 사용시 ROLLBACK
END CATCH
```

* TRAN 사용의 주의점
    * TRAN 안에는 꼭 원자적으로 실행될 애들만 넣자
    * 아에 성공 혹은 실패

```sql
BEGIN TRAN
    INSERT INTO accounts VALUES(1, 'rookiss', 100, GETUTCDATE())
```

```sql
SELECT *
FROM accounts
-- 여기서 멈춰있는다
-- ROLLBACK이나 COMMIT을 안한상태로 account를 SELECT할 수 없음.
    -- 이게 성능적으로 엄청난 성능 저하를 불러올 수 있다.
    -- TRAN은 꼭 원자적으로 사용!
```