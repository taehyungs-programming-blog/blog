---
layout: default
title: "2. NoSQL 이란?"
parent: "MongoDB 💾"
grand_parent: "Python"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## RDBMS(SQL) Vs. NoSQL

😎 우선 가장 이해하기 쉬운 개념적 측면을 먼저 설명한다.

* **RDBMS(SQL)** : 정해진 규격이 존재한다.
* **NoSQL(Not Only SQL)** : 정해진 규격이 없다.

```
# Example

# 만약 게임의 캐릭 DB가 있다면 내부적으로 HP, MP, Skill 등의 정보를 보관하고 있어야 한다.

Character --- HP
           ㄴ MP
           ㄴ Skill
              ...
```

😎 그러데 만약 새로운 직업이 생겨서 HP, MP, Skill 이외에 Money라는 항목이 추가 된다면?<br>
😎 기존의 Character DB에서 모든 DB의 Money를 추가해야 할까? 그러다 또 항목이 추가된다면???<br>
😎 이런 일을 방지하기 위해 **NoSQL**이 탄생했다<br>
😎 NoSQL은 정해진 규격이 없고 필요에 따라 DB를 만들어 사용한다.(자세한건 이후 강의에 참조!)

* 조금 더 SQL과 NoSQL의 차이점을 보자면 ...

| SQL        | NoSQL          |
|:-------------|:------------------|
| 정해진 규격 존재 | 정해진 규격 없음 |
| Join 가능 | Join 불가능 |
| 트랜잭션 사용 | 트랜잭션 없음 |
| 분산처리 어려움 | 분산처리 쉬움 |

😎 또한 **Read가 대부분인 DB**의 경우 사실 정해진 틀을 제공하는 SQL을 써도 사실 그렇게 성능적 이슈가 없다<br>
😎 하지만 최근 Web기준 읽기만하는 기능보단 **사용자가 Write하는 기능이 중요**해졌다(분산되어 처리 필요)<br>
😎 따라서 **Write가 많이 필요한** 최근 Web기준으로 NoSQL이 더 유리해졌다.

---

## MongoDB란?

😎 mongoDB는 JSON을 기반으로 데이터를 관리하게 된다.

```json
{
    "_id": ObjectId{"1234"},
    "hp": "100",
    "mp": "300",
    "name": { "first": "Hi", "last": "there" }
}
```

😎 정말 직관적이지 않은가? 심지어 xml보다 속도도 빠르다!<br>
😎 다음으로 MongoDB에서 사용하는 중요한 용어정리를 해보겠다.

```
# RDBMS(SQL) 를 기준으론 Table이 존재하고 table의 key를 보고 Table간의 상관관계를 조사

-----------        -------------        ------------
|  캐릭터 |        | inventory |        |    ,,,   |
-----------        -------------        ------------
| id(key) |  ----> |   id(key) |  ----> |  id(key) |
|   name  |        |   item    |
| server  |        |    ...    |
|   ...   |

# 대략 위 처럼 처리된다.
```

😎 그에 반해 NoSQL의 경우 id를 기반으로 Collection을 하나 만들고 그 내부애 Document로 필요한 정보를 넣게 된다.

```
# 전체가 Collection이고
# 캐릭터, inventory가 Document가 된다.

id ------ 캐릭터 ------- name
        |             ㄴ server
        |
        ㄴinventory ---- item               

```

* 용어정리
    * **Collection** : MongoDB의 Document의 집합
    * **Document** : Collection의 SubSet

| RDBMS | MongoDB |
|:-------------|:------------------|
| Database로 데이터 관리 | Database로 데이터 관리 |
| 내부에 Table 존재 | Collection 생성 |
| Table내 Tuple/ Row 존재 | Collection은 Document로 관리 |