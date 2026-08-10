---
layout: default
title: "12. [Crawlling] 3"
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

🥽 지난코드

```py
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

print("... before parsing ...")

for actor in actors:
    print(actor.text)

print("... after parsing ...")

# 정규 표현식으로 이름만 뽑기
for actor in actors:
    newname = re.sub("\(\w*\)", "", str(actor.text))  # '\(\w*\)' : 괄호안의( \(\) ) 어떤문자( w* ) 든 '' 삭제해 달라
    print(newname)

print("... actor info print ...")

# 배우 상세정보 추출(정규표현식 이용)
for actor in actors:
    # print('http://www.cine21.com' + actor.select_one('a').attrs['href'])
    actor_url = 'http://www.cine21.com' + actor.select_one('a').attrs['href']
    response_actor = requests.get(actor_url)
    soup_actor = BeautifulSoup(response_actor.content, 'html.parser')
    actor_info = soup_actor.select_one('ul.default_info')
    actor_details = actor_info.select('li')
    for item in actor_details:
        # print(item)
        print(item.select_one('span.tit').text)
        actor_value = re.sub('<span.*?>.*?</span>','',str(item))
        print(re.sub('<.*?>','',str(actor_value)))
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/python/nosql/nosql-8-1.png"/>
</p>

🥽 이제 파싱한 데이터를 mongodb에 넣어보자.<br>
🥽 우선 파싱한 데이터를 list, dict로 만들어서 관리해보자.

```py
# 배우 상세정보 추출 list, dict 관리
actors_detail_info_list = list()

for actor in actors:
    # print('http://www.cine21.com' + actor.select_one('a').attrs['href'])
    actor_url = 'http://www.cine21.com' + actor.select_one('a').attrs['href']
    response_actor = requests.get(actor_url)
    soup_actor = BeautifulSoup(response_actor.content, 'html.parser')
    actor_info = soup_actor.select_one('ul.default_info')
    actor_details = actor_info.select('li')

    actor_info_dict = dict()

    for item in actor_details:
        actor_item_filed = item.select_one('span.tit').text
        actor_value = re.sub('<span.*?>.*?</span>','',str(item))
        actor_item_value = re.sub('<.*?>','',str(actor_value))
        actor_info_dict[actor_item_filed] = actor_item_value

    actors_detail_info_list.append(actor_info_dict)

print(actors_detail_info_list)
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/python/nosql/nosql-8-2.png"/>
</p>

🥽 여기까지하면 mongodb에 넣을 준비는 됐고 ... <br>
🥽 이것만 넣기는 아쉬우니 몇 가지 더 넣어보자.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/python/nosql/nosql-8-3.png"/>
</p>

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/python/nosql/nosql-8-4.png"/>
</p>

🥽 `num_info` 아래 `<strong>`을 보면 될꺼 같다.

```py
# 배우 흥행지수 뽑기
actors = soup.select('li.people_li div.name')
hits = soup.select('ul.num_info > li > strong')

print(actors)
print(hits)
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/python/nosql/nosql-8-5.png"/>
</p>

🥽 약간 파싱이 필요해 보인다.

```py
# 배우 흥행지수 뽑기
actors = soup.select('li.people_li div.name')
hits = soup.select('ul.num_info > li > strong')

# print(actors)
# print(hits)

for index, actor in enumerate(actors):
    # print(actor)
    print(re.sub('\(\w*\)','',actor.text))
    # print(hits[index])
    print(int(hits[index].text.replace(',', '')))
```
