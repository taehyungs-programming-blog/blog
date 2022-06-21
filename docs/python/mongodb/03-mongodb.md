---
layout: default
title: "3. MongoDB(Robo 3T) 간단 사용"
parent: "MongoDB 💾"
grand_parent: "Python"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## MongoDB(Robo 3T) 간단 사용

👺 [Robo 3T](https://robomongo.org/)환경에서 MongoDB를 간단히 사용해 보자.<br>
👺 localhost에 MongoDB가 설치되었다면 localhost로 Connection을 하자<br>

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/python/nosql/nosql-3-11.png"/>
</p>

👺 LocalConnection 우클릭 👉 Create Database

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/python/nosql/nosql-3-1.png"/>
</p>

👺 mydb라는 이름으로 만들어 보자

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/python/nosql/nosql-3-2.png"/>
</p>

👺 Collections 폴더 우클릭 👉 Create Collection... 👉 mycollection라는 이름으로 만들어 보자.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/python/nosql/nosql-3-3.png"/>
</p>

👺 Collection 우클릭 👉 Insert Document

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/python/nosql/nosql-3-4.png"/>
</p>

👺 JSON을 기입할 수 있는 에디터가 나타난다.<br>
👺 JSON 형식으로 데이터를 기입 후 확인

```json
{
    ID: "C-01",
    name: "taehyung",
    age: 34
}
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/python/nosql/nosql-3-5.png"/>
</p>

👺 신기한건 **_id**는 넣지 않았는데 들어가 있는데 Document의 유효성을 위해 기본적으로 들어간다.<br>
👺 하나 더 넣어 보자.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/python/nosql/nosql-3-6.png"/>
</p>

---

## (Robo 3T) Command 입력해보기

🧔 자신의 DB 우클릭 -> Open Shell

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/python/nosql/nosql-3-7.png"/>
</p>

```s
$ show dbs
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/python/nosql/nosql-3-8.png"/>
</p>

🧔 현재 mongodb에 생성이 되어있는 db를 모두 보여준다.

```s
# mydb를 사용하겠다 명령
$ use mydb

# collections를 보여달라
$ show collections
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/python/nosql/nosql-3-9.png"/>
</p>

```s
# collection에 있는 모든 document를 보여달라
$ db.mycollection.find()
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/python/nosql/nosql-3-10.png"/>
</p>

```s
# db정보 출력
$ db.stats()
```

```s
# collection 만들어 보기
    # capped : db의 capacity를 100으로 미리 잡아달라
$ db.createCollection("newCollection", {capped:true, size:100})

# 참고로 capped를 확인가능
$ db.newCollection.iscapped()
```