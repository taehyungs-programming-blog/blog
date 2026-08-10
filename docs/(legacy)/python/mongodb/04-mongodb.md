---
layout: default
title: "4. CRUD"
parent: "MongoDB 💾"
grand_parent: "Python"
great_grand_parent: "Legacy Archive"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## CRUD

👩 SQL을 간단히 정리하자면 CRUD 네 가지로 정리 될 수 있다.

* Create
* READ
* Update
* Delete

👩 동일하게 NoSQL에서도 CRUD를 어떻게 처리하는지 보자.

### Collection (TABLE)

```sql
CREATE TABLE people (
    id MEDIUMINT NOT NULL AUTO_INCREMENT,
    user_id Varchar(30),
    age Number,
    status char(1),
    PRIMARY KEY (id)
)
```

👩 조금 해석하자면 `PRIMARY KEY (id)`를 통해 id를 primary key로 만들어 데이터를 구분해 달라<br>
👩 이걸 NoSQL에서는 어떻게 표현할까?

```json
db.people.insertOne ({
    user_id: "user1",
    age: 55,
    status: "A"
})
```

👩 ??? Primary key는? MongoDB에서 자동으로 _id값이 들어가며 그 _id값이 Primary Key가 된다.

---

## 입력 (Create)

* `insertOne` : 한 개의 document 생성
* `insertMany` : 여러개의 document 생성

```json
db.people.insertOne (
    {
        user_id: "user1",
        age: 55,
        status: "A"
    }
)
```

```s
$ use <db name>
$ db.createCollection("users")
$ db.users.insertOne(
    {
        user_id: "user1",
        age: 55,
        status: "A"
    }
)
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/python/nosql/nosql-4-1.png"/>
</p>

👩 SQL 기준으로 보자면

```sql
INSERT INTO people(user_id, age, status)
VALUES ("user1", 45, "A")
```

### Create Example

👩 여러 document를 넣어보자.

```s
$ use <db name>
$ db.createCollection("users")
$ db.users.insertMany(
    [
        { user_id: "user1", age: 55, status: "A" },
        { user_id: "user2", age: 40, status: "C" },
        { user_id: "user3", age: 50, status: "B" },
        { user_id: "user4", age: 15, status: "D" }
    ]
)
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/python/nosql/nosql-4-2.png"/>
</p>

---

## 읽기

* `findOne` : 매칭되는 한개의 document 출력
* `find` : 매칭되는 여러개의 documents 출력

```json
db.users.find(
    { age: {$gt: 18} },
    { name: 1, address: 1 }
).limit(5)
```

* `{ name: 1, address: 1 }` : 데이터 중 name과 address만 가져와 달라
    * (주의) `{ _id: 0 }` : _id는 기본으로 들어가기에 빼고자 한다면 명시적으로 알려줘야 함
* `.find({ status: "A" })` : status가 A일때만 가져와 달라
* `.find({ $or: [ { status: "A"} , { age: 50}]})` : or연산 가능

### Read Example

```json
db.users.find({}, {user_id: 1, status: 1})
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/python/nosql/nosql-4-3.png"/>
</p>

```json
db.users.find({status: "A"})
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/python/nosql/nosql-4-4.png"/>
</p>

### 비교 문법

* `$eq` : =
* `$gt` : > (`db.example.find({ age: { $gt: 25 } })`)
* `$gte` : >=
* `$in` : (`db.example.find({ age: { $in: [5, 15] } })`) 5 혹은 15
* `$lt` : <
* `$lte` : <= (`db.example.find({ age: { $gt: 25, $lte: 50 } })`)
* `$ne` : !=
* `$nin` : not in
* 특정 문자 포함 : 
    * `db.example.find({ user_id: /bc/ })` bc가 포함된 문자
    * `db.example.find({ user_id: { $regex: /^bc/} })`

---

## 수정

* `updateOne` : 한 개의 document 업데이트 (가장 먼저 검색된 document만 수정됨)
* `updateMany` : 여러개의 document 업데이트

```json
db.users.updateMany(
    { age: { $lt: 18 } },
    { $set: { status: "reject" }}
)
```

* `age: { $lt: 18 }` age가 18보다 작은애들을 `$set: { status: "reject" }` status를 reject로 변경해 달라

```json
db.users.updateMany(
    { age: { $lt: 18 } },
    { $inc: { age: 3 }}
)
```

* age를 3만큼 증가

---

## 삭제

* `removeOne` : 한 개의 document 삭제
* `removeMany` : 여러개의 document 삭제

```json
db.users.deleteMany(
    { status: "reject" }
)
```

* `db.users.deleteMany({})` 모두 삭제