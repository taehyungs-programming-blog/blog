---
layout: default
title: "6. pymongo 문법 - 1"
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

* [Get Code 🌍](https://github.com/EasyCoding-7/pymongo_examples/blob/main/pymongo_1/pymongo_1/pymongo_5.py)

---

## mongo db 데이터 쌓기

🎈 시작하기 전 기존의 코드로 mongodb에 데이터를 쌓아두자<br>
🎈 아래는 기존코드(한번 돌리면 됨 / 약간 시간이 걸리니 알아두자.)

```py
import configparser
import pymongo
import requests
from bs4 import BeautifulSoup
import re                       # regular expression

# 배우 상세정보 추출 list, dict 관리
actors_detail_info_list = list()

# read aws ip and port info
iniFile = 'aws_address.ini'
ini_parser = configparser.ConfigParser()
ini_parser.read(iniFile)

# connection db, collection
# conn = pymongo.MongoClient(str(ini_parser['aws']['ip']), int(ini_parser['aws']['port']))
conn = pymongo.MongoClient('localhost', 27017)
actor_db = conn.cine21_actor
actor_collection = actor_db.actor_collection
actor_collection.delete_many({})

# crawlling
url = "http://www.cine21.com/rank/person/content"
post_data = dict()
post_data['section'] = 'actor'
post_data['period_start'] = '2020-03'
post_data['gender'] = 'all'

for pindex in range(1, 21):
    post_data['page'] = pindex

    res = requests.post(url, data=post_data)

    # parsing
    soup = BeautifulSoup(res.content, 'html.parser')
    actors = soup.select('li.people_li div.name')

    # 배우 흥행지수 뽑기
    actors = soup.select('li.people_li div.name')
    hits = soup.select('ul.num_info > li > strong')
    movies = soup.select('ul.mov_list')
    rankings = soup.select('li.people_li > span.grade')

    for index, actor in enumerate(actors):
        # 배우이름(actor_name), hit점수(actor_hits), 대표작 리스트(movie_titles_list) 파싱
        actor_name = re.sub('\(\w*\)','',actor.text)
        actor_hits = int(hits[index].text.replace(',', ''))
        actor_rank = int(rankings[index].text)
        movie_titles = movies[index].select('li a span')
        movie_titles_list = list()
        for movie in movie_titles:
            movie_titles_list.append(movie.text)

        # 배우 상세정보(actor_url)
        actor_url = 'http://www.cine21.com' + actor.select_one('a').attrs['href']
        response_actor = requests.get(actor_url)
        soup_actor = BeautifulSoup(response_actor.content, 'html.parser')
        actor_info = soup_actor.select_one('ul.default_info')
        actor_details = actor_info.select('li')

        actor_info_dict = dict()
        actor_info_dict['배우이름'] = actor_name
        actor_info_dict['hits'] = actor_hits
        actor_info_dict['rank'] = actor_rank
        actor_info_dict['출연작'] = movie_titles_list

        for item in actor_details:
            actor_item_filed = item.select_one('span.tit').text
            actor_value = re.sub('<span.*?>.*?</span>','',str(item))
            actor_item_value = re.sub('<.*?>','',str(actor_value))
            actor_info_dict[actor_item_filed] = actor_item_value

        actors_detail_info_list.append(actor_info_dict)

# print(actors_detail_info_list)
actor_collection.insert_many(actors_detail_info_list)
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/python/nosql/nosql-11-1.png"/>
</p>

---

## 데이터 출력 (find)

🎈 우선 데이터가 잘 읽어와 지는지 확인부터 하자

```py
import configparser
import pymongo

# read aws ip and port info
iniFile = 'aws_address.ini'
ini_parser = configparser.ConfigParser()
ini_parser.read(iniFile)

# connection db, collection
# conn = pymongo.MongoClient(str(ini_parser['aws']['ip']), int(ini_parser['aws']['port']))
conn = pymongo.MongoClient('localhost', 27017)
actor_db = conn.cine21_actor
actor_collection = actor_db.actor_collection

# 하나출력
print(actor_collection.find_one({}))

# 갯수지정해 출력
docs = actor_collection.find({}).limit(3)
for doc in docs:
    print(doc)
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/python/nosql/nosql-11-2.png"/>
</p>

---

## column명 수정 (rename)

```py
actor_collection.update_many({}, {'$rename': {'다른 이름':'새로운이름'}} )
```

---

## 정렬 (sort)

```py
docs = actor_collection.find({}).sort('생년월일', pymongo.DESCENDING).limit(20)
for doc in docs:
    print(doc)
```

---

## 특정 필드가 있는지 확인 (exists)

```py
docs = actor_collection.find( {'특기': {'$exists': True}} ).sort('흥행지수').limit(5)
for doc in docs:
    print(doc)
```

```py
# 생년월일이 없는 document의 배우이름과 _id만 출력해 달라
docs = actor_collection.find( {'생년월일': {'$exists': False}}, {'배우이름':1, '_id':1} )
for doc in docs:
    print(doc)
```

---

## 범위 검색

* [Get Code 🌍](https://github.com/EasyCoding-7/pymongo_examples/blob/main/pymongo_1/pymongo_1/pymongo_6.py)

```py
docs = actor_collection.find( {'hits': {'$gte': 10000}})
for doc in docs:
    print(doc)
```

```py
# 배우이름만 출력
docs = actor_collection.find( {'hits': {'$gte': 10000}}, {'배우이름':1, '_id':0})
for doc in docs:
    print(doc)
```

```py
# 출연영화가 극한직업이며, 배우이름만 출력
docs = actor_collection.find( {'hits': {'$gte': 10000}, '출연작':'극한직업' }, {'배우이름':1, '_id':0})
for doc in docs:
    print(doc)
```

```py
# or 연산
docs = actor_collection.find( {'$or': [{'출연작':'극한직업'}, {'출연작':'더 킹'}]}, {'배우이름':1, '_id':0})
for doc in docs:
    print(doc)
```

```py
# or 연산2
docs = actor_collection.find( { 'hits': { '$gte':100 },  '$or' : [ {'출연작':'극한직업'}, {'출연작':'더 킹'} ] }, { '배우이름':1, '_id':0 } )
for doc in docs:
    print(doc)
```
