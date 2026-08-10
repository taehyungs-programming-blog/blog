---
layout: default
title: "10. [Crawlling] 1"
parent: "MongoDB 💾"
grand_parent: "Python"
great_grand_parent: "Legacy Archive"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌍](https://github.com/EasyCoding-7/pymongo_examples/blob/main/pymongo_1/pymongo_1/pymongo_2.py)

---

## WEB 페이지 크롤링 기초

* GET, POST 방식에 따라 크롤링 방법이 달라진다.
    * **GET** : url 자체가 달라지기에 달라진 url로 request를 보내고 받으면됨.
    * **POST** : url은 달라지지 않기에 개발자 도구에서 확인해야함.

🧸 네이버 뉴스의 경우 GET방식으로 뉴스페이지를 누를때마다 url이 달라짊을 확인할 수 있다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/python/nosql/nosql-6-1.png"/>
</p>

🧸 Cine21의 경우 POST방식으로 url은 달라지지 않고 페이지만 변경된다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/python/nosql/nosql-6-2.png"/>
</p>

🧸 개발자 도구 -> 네트워크 -> content 확인시<br>
🧸 POST 데이터 확인가능

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/python/nosql/nosql-6-3.png"/>
</p>

```
요청 URL : http://www.cine21.com/rank/person/content
요청 메서드 : POST

section: actor
period_start: 2020-03
gender: all
page: 1
```

🧸 그럼 이 POST를 동일하게 만들어서 request를 만들어보자

```py

import configparser
import pymongo
import requests
from bs4 import BeautifulSoup
import re                       # regular expression

# connection db, collection
conn = pymongo.MongoClient(str(ini_parser['aws']['ip']), int(ini_parser['aws']['port']))
actor_db = conn.cine21_actor
actor_collection = actor_db.actor_collection

# crawlling
url = "http://www.cine21.com/rank/person/content"
post_data = dict()
post_data['section'] = 'actor'
post_data['period_start'] = '2020-03'
post_data['gender'] = 'all'
post_data['page'] = 1

res = requests.post(url, data=post_data)

# parsing
soup = BeautifulSoup(res.content, 'html.parser')
actors = soup.select('li.people_li div.name')

print("... before paring ...")

for actor in actors:
    print(actor)

print("... after paring ...")

# 정규 표현식으로 이름만 뽑기
for actor in actors:
    newname = re.sub("\(\w*\)", "", str(actor))  # '\(\w*\)' : 괄호안의( \(\) ) 어떤문자( w* ) 든 '' 삭제해 달라
    print(newname)
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/python/nosql/nosql-6-4.png"/>
</p>
