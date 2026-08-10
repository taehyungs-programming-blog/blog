---
layout: default
title: "7. pymongo 문법 - 2"
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

* [Get Code 🌍](https://github.com/EasyCoding-7/pymongo_examples/blob/main/pymongo_1/pymongo_1/pymongo_7.py)

---

## nor 연산

```py
# nor
docs = actor_collection.find( 
    { 
        '$nor' : 
        [ 
            { 'hits' : { '$gte': 10000 } }, 
            { 'hits' : { '$lt': 1000 } } 
        ] 
    }, 
    { '배우이름':1, '_id':0 } 
)
for doc in docs:
    print(doc)
```

---

## in/ nin

```py
# in
docs = actor_collection.find( 
    { 
        'hits' : 
        {
            # '$in': [9182]
            '$nin': [9182]
        }
    }, 
    { '배우이름':1, '_id':0 } 
).limit(3)
for doc in docs:
    print(doc)
```

---

## skip

* **skip** : 검색결과 n개를 건너뜀

```py
# skip
docs = actor_collection.find( 
    { 
        'hits' : 
        {
            '$gte': 10000
        }
    }
)
for doc in docs:
    print(doc)

docs = actor_collection.find( 
    { 
        'hits' : 
        {
            '$gte': 10000
        }
    }
).skip(3)
for doc in docs:
    print(doc)
```

---

## list 내부 검색

```py
docs = actor_collection.find( 
    { 
        # '$or' : 
        '$and' : 
        [
            { '출연작': '극한직업' },
            { '출연작': '사바하' }
        ]
    }
)
for doc in docs:
    print(doc)
```

```py
# 이렇게 표현해도 된다.
docs = actor_collection.find( 
    { 
        '출연작' : 
        {
            '$all' :
            [
                '극한직업',
                '사바하'
            ]
        }
    }
)
for doc in docs:
    print(doc)
```

```py
# index번호로 확인가능
docs = actor_collection.find( 
    { 
        '출연작.0' :    # index가 0인게
        '사바하'        # 사바하일경우 출력
    }
)
for doc in docs:
    print(doc)
```

```py
# 출연작이 5개인경우
docs = actor_collection.find( 
    { 
        '출연작' : { '$size': 5 }
    }
)
for doc in docs:
    print(doc)
```

---

## elemMatch

```py
# 새로운 sample collection을 만들고
elemmatch_sample = actor_db.sample

# 테스트용 데이터를 넣자
elemmatch_sample.insert_many([
    {'results': [82, 85, 88]},
    {'results': [75, 88, 91]}
])
```

여기서 만약 90이상 85미만을 갖는 document를 찾고싶다면?

```py
docs = elemmatch_sample.find({
    'results': { '$gte': 90, '$lt':85 }
})
# 이렇게 하면 단점이 90이상도 있고, 85미만도 있는 document만 출력
```

```py
# 둘 중 하나만 있으면 출력하게 하고싶다면
docs = elemmatch_sample.find({
    'results': { '$elemMatch': { '$gte': 90, '$lt':85 }}
})
```