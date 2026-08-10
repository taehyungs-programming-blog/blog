---
layout: default
title: "11. [Crawlling] 2"
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

🚗 우선 지난 코드는 다음과 같고

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
```

🚗 실행해 보면 ...

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/python/nosql/nosql-7-1.png"/>
</p>

🚗 해보고 싶은것은 배우의 상세페이지에 정보를 Crawlling하는 것이다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/python/nosql/nosql-7-2.png"/>
</p>

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/python/nosql/nosql-7-3.png"/>
</p>

🚗 좀 다른 방법으로 crawlling을 해야하는데

```py
# 그냥 출력해보면
for actor in actors:
    print(actor)
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/python/nosql/nosql-7-4.png"/>
</p>

* `<div class="name"><a href="/db/person/info/?person_id=57423">김윤석(7편)</a></div>`
    * a 태그 아래 href 속성의 데이터가 필요하다

```py
for actor in actors:
    print(actor.select_one('a').attrs['href'])
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/python/nosql/nosql-7-5.png"/>
</p>

```py
for actor in actors:
    # 이런식으로 링크를 만들수 있다.
    print('http://www.cine21.com' + actor.select_one('a').attrs['href'])
```

```py
# 배우 상세정보 추출
for actor in actors:
    # print('http://www.cine21.com' + actor.select_one('a').attrs['href'])
    actor_url = 'http://www.cine21.com' + actor.select_one('a').attrs['href']
    response_actor = requests.get(actor_url)
    soup_actor = BeautifulSoup(response_actor.content, 'html.parser')
    actor_info = soup_actor.select_one('ul.default_info')
    actor_details = actor_info.select('li')
    for item in actor_details:
        print(item)
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/python/nosql/nosql-7-6.png"/>
</p>

🚗 여기까지 파싱하면 위와 같이 나오는데 좀 더 파싱이 필요하다

---

## 정규표현식 이용

🚗 tit 필드안의 값까지는 아래와 같이 뽑을 수 있음

```py
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
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/python/nosql/nosql-7-7.png"/>
</p>

🚗 문제는 필드의 값을 뽑는 방법이 없다는 것이다. -> 정규표현식을 이용

* Greedy : `.*`
    * `.`은 정규표현식에서 \n를 제외한 모든 한 개의 문자를 의미한다.
    * `*`는 앞 문자가 0번 혹은 그 이상 반복되는 패턴
    * `.*`는 문자가 0번 혹은 그 이상 반복(모든 문자의미)
        * 그럼 `<.*>` 이렇게 표현시
        * `<li><span class="tit">원어명</span>주지훈</li>` 이걸 어떻게 파싱할까?? 
        * `<li>` , `<span class="tit">` 이렇게 따로 따로 파싱?
        * Nope `<li><span class="tit">원어명</span>주지훈</li>`를 하나로 인식한다.
        * 따로 인식시키고자 한다면 Non-Greedy이용
* Non-Greedy : `.*?`
* [정규표현식 테스트 사이트 🌍](https://regexr.com)

🚗 정규표현식에 매칭이 되는 부분이 표시가 된다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/python/nosql/nosql-7-8.png"/>
</p>

```py
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
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/python/nosql/nosql-7-9.png"/>
</p>