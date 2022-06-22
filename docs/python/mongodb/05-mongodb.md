---
layout: default
title: "5. pymongo"
parent: "MongoDB 💾"
grand_parent: "Python"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## python 라이브러리 사용법(필요하다면 보세요.)

```py
# math 라이브러리 안에 sqrt, factorial만 import하겠다.
from math import sqrt, factorial

# math. 없이 사용가능
print(sqrt(5))
```

```py
# math를 dave로 쓰게해 달라
import math as dave
from math import factorial as f

print(dave.sqrt(5))
```

---

## pymongo connection

```s
$ pip install pymongo
```

* [Get Code 🌍](https://github.com/EasyCoding-7/pymongo_examples/tree/main/pymongo_1)

```py
import pymongo

# connection 맺기
connection = pymongo.MongoClient('[ip]', [port])

# db에 연결(없다면 생성됨)
db = connection.test # connection["test"] 이것도 가능.

# 연결상태 출력
print(db)
# Database(MongoClient(host=['ip:port'], document_class=dict, tz_aware=False, connect=True), 'users')

# db의 이름출력
print(db.name)
```

---

## collection 만들기

```py
import pymongo

# connection 맺기
connection = pymongo.MongoClient('[ip]', [port])

# db에 연결(없다면 생성됨)
db_test1 = connection.test1 # connection["test1"] 이것도 가능.
db_collection = db_test1.db_collection # db_test1["db_collection"] 이것도 가능.

print(db_collection)
```

🦐 이쯤에서 생기는 의문점? Robo 3T에서 확인해 보니 db랑 collection이 안생겼는데??

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/python/nosql/nosql-5-1.png"/>
</p>

🦐 **(주의)** Data를 Insert하기 전까지는 db와 collection이 생성되지 않음!

---

## INSERT

```py
import pymongo

# connection 맺기
connection = pymongo.MongoClient('[ip]', [port])

# db에 연결(없다면 생성됨)
db_test1 = connection.test1             # connection["test1"] 이것도 가능.
db_collection = db_test1.db_collection  # db_test1["db_collection"] 이것도 가능.

# INSERT
post = {"user":"user01", "guild":"superman", "type":"knight"}
db_collection.insert_one(post)

result = db_collection.insert_many(
    [
        {"user":"user01", "guild":"superman", "type":"knight"},
        {"user":"user02", "guild":"badman", "type":"none"},
        {"user":"user03", "guild":"ultraman", "type":"elf"}
    ]
)

# INSERT 결괄르 받을수도 있음
print(result)
print(result.inserted_id)       # object id 출력
print(db_collection.count())    # document 개수 출력
```

🦐 MongoDB 버전에러가 발생시? <br>
🦐 우선은 MongoDB의 버전을 확인해 본다. `db.version()`

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/python/nosql/nosql-5-2.png"/>
</p>

🦐 pymongo버전을 다운그레이드 시키길 바란다(4.0이상부터 MongoDB 3.4 이하 지원안함.)

* [pymongo 버전별 지원 mongodb 🌍](https://docs.mongodb.com/drivers/pymongo/)

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/python/nosql/nosql-5-3.png"/>
</p>


### list를 만들어 넣어보기

```py
data = list()
data.append( {'name': 'n1', 'age': 10 } )
data.append( {'name': 'n2', 'age': 11 } )
data.append( {'name': 'n3', 'age': 12 } )
data.append( {'name': 'n4', 'age': 13 } )
data.append( {'name': 'n5', 'age': 14 } )

collection.insert_many(data)
```

---

## 검색(FIND)

```py
import pymongo

# connection 맺기
connection = pymongo.MongoClient('[ip]', [port])

# db에 연결(없다면 생성됨)
db_test1 = connection.test1             # connection["test1"] 이것도 가능.
db_collection = db_test1.db_collection  # db_test1["db_collection"] 이것도 가능.

# INSERT
post = {"user":"user01", "guild":"superman", "type":"knight"}
db_collection.insert_one(post)

# FIND
db_collection.find_one({"user":"user01"})
find_list = db_collection.find({"user":"user01"})

for item in find_list:
    print(item)
```

```py
# 특정 조건으로 sort가능
db_collection.find({}).sort("age")
```

---

## 데이터 수정(UPDATE)

```py
import pymongo

# connection 맺기
connection = pymongo.MongoClient('[ip]', [port])

# db에 연결(없다면 생성됨)
db_test1 = connection.test1             # connection["test1"] 이것도 가능.
db_collection = db_test1.db_collection  # db_test1["db_collection"] 이것도 가능.

# INSERT
post = {"user":"user01", "guild":"superman", "type":"knight"}
db_collection.insert_one(post)

# UPDATE
    # 아래 데이터를 업데이트할 예정
db_collection.find_one({"user":"user01"})

test_collection.update_one( {
        "user":"user01",
        {
            "$set" : {"test":"update test"}
        }
    }
)
```

---

## 삭제 (DELETE)

```py
import pymongo

# connection 맺기
connection = pymongo.MongoClient('[ip]', [port])

# db에 연결(없다면 생성됨)
db_test1 = connection.test1             # connection["test1"] 이것도 가능.
db_collection = db_test1.db_collection  # db_test1["db_collection"] 이것도 가능.

# INSERT
post = {"user":"user01", "guild":"superman", "type":"knight"}
db_collection.insert_one(post)

# DELETE
db_collection.delete_one( { "user":"user01" } )
db_collection.delete_many( { "user":"user01" } )
```

```py
# 6이상만 삭제
db_collection.delete_many( { "num": { "$gte": 6 } } )
```