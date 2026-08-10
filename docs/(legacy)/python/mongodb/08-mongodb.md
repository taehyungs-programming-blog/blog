---
layout: default
title: "8. pymongo index"
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

* **INDEX** : 검색을 빠르게 해주는 파라미터
    * 기본 INDEX는 **_id** 이다.
    * Single(단일), Compound(복합)으로 구성될 수 있음.

🕶 그럼 한 번 코드로 확인해 보자.<br>
🕶 기존코드를 활용.

* [Get Code 🌍](https://github.com/EasyCoding-7/pymongo_examples/blob/main/pymongo_1/pymongo_1/pymongo_8.py)

```py
import configparser
import pymongo

# read aws ip and port info
iniFile = 'aws_address.ini'
ini_parser = configparser.ConfigParser()
ini_parser.read(iniFile)

# connection db, collection
conn = pymongo.MongoClient(str(ini_parser['aws']['ip']), int(ini_parser['aws']['port']))
actor_db = conn.cine21_actor
actor_collection = actor_db.actor_collection
```

```py
# INDEX 생성(Single)
actor_collection.create_index('배우이름')
```

```json
{
    '_id_': 
    {
        'v': 2, 
        'key': [('_id', 1)], 
        'ns': 'cine21_actor.actor_collection'
    }, 
    '배우이름_1': 
    {
        'v': 2, 
        'key': [('배우이름', 1)], 
        'ns': 'cine21_actor.actor_collection'
    }
}
```

* key :
    * field name
    * direction (1 : ASCENDING / 2 : DESCENDING)

```py
# 이렇게도 선언가능
actor_collection.create_index(['배우이름', pymongo.DESCENDING])
```

```py
# index로 검색시 문자열 단위로 검색하고 싶다면
docs = text_collection.find(
    {
        '$text' :
        {
            '$search' : '\"coffee shop\"'
            # coffee shop을 포함시 출력
        }
    }
)
```

```py
# 대소문자 구분할 경우
docs = text_collection.find(
    {
        '$text' :
        {
            '$search' : 'coFFee',
            '$caseSensitive': True
        }
    }
)
```

---

🕶 이게 정말 성능에 효과가 있나???<br>
🕶 사실 데이터가 적을 경우 성능의 효과는 미미하다.<br>
🕶 그럼 이걸 왜 써야하지?? 👉 아래와 같은 케이스가 있기 때문이다.

```py
# text로 indexing이 되어있지 않기에 아래서 에러 리턴
docs = actor_collection.find( {'$text' : {'$search' : '왕' } } ) # error
for doc in docs:
    print(doc)
```

```py
# text index를 지정해보자.
actor_collection.create_index( [ ('출연작', 'text') ] )

# 재시도
docs = actor_collection.find( 
    {
        '$text' : 
        {
            '$search' : '왕' 
        } 
    } 
)
for doc in docs:
    print(doc)
```

---

## Compound(복합) Field Indexing

* [Get Code 🌍](https://github.com/EasyCoding-7/pymongo_examples/blob/main/pymongo_1/pymongo_1/pymongo_9.py)

```py
import configparser
import pymongo

# read aws ip and port info
iniFile = 'aws_address.ini'
ini_parser = configparser.ConfigParser()
ini_parser.read(iniFile)

# connection db, collection
conn = pymongo.MongoClient(str(ini_parser['aws']['ip']), int(ini_parser['aws']['port']))
actor_db = conn.cine21_actor
actor_collection = actor_db.actor_collection

# 테스트를 위해 처음은 INDEX 삭제해 준다.
actor_collection.drop_indexes()

# compound indexing
actor_collection.create_index(
    [
        ('출연작', pymongo.TEXT),
        ('직업', pymongo.TEXT),
        ('hits', pymongo.TEXT)
    ]
)

# 검색해 보자면
docs = actor_collection.find(
    {
        '$text' : 
        {
            '$search' : '가수'
        }
    }
)
for doc in docs:
    print(doc)
```
