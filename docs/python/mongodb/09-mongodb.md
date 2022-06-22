---
layout: default
title: "9. 데이터 분석"
parent: "MongoDB 💾"
grand_parent: "Python"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌍](https://github.com/EasyCoding-7/pymongo_examples/blob/main/pymongo_1/pymongo_1/pymongo_10.py)

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

# indexing을 하지 않았는데 아래가 동작할까?

# 정규식을 이용해 문자열 검색
result = actor_collection.find( {'출연작' : { '$regex' : '함께' }})
for rec in result:
    print(rec)
    # 잘 동작한다. 
    # regex를 사용하기 때문이다.
```

```py
docs = text_collection.find(
    {
        'name' :
        {
            '$regex' : 'Co.*'
            # Co이후 어떤문자라도 나올 시 출력
        }
    }
)
```