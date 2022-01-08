---
layout: default
title: 2. 개발환경구성
parent: (Django)
grand_parent: Python
nav_order: 5
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## 개발환경구성

😺 디버깅, 개발환경 구성의 편의성을 위해서 [PyCharm Community](https://www.jetbrains.com/ko-kr/pycharm/download/#section=windows)를 기반으로 개발환경을 구성함(유튜부와 다름)

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/python/django/basic-2-1.png" style="border-radius:5%;border:1px solid #e6e1e8"/>
</p>

<br>

😺 터미널에서 pip에 설치된 라이브러리를 확인한다 (`$ pip list`)

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/python/django/basic-2-2.png" style="border-radius:5%;border:1px solid #e6e1e8"/>
</p>

<br>

😺 `pip`, `setuptools`, `wheel`만 설치되어있고 `django`는 설치되어 있지 않기에 Django를 설치해 본다 `$ pip install django`

😺 설치가 완료되었다면 `$ django-admin` 명령을 통해 django가 잘 설치되었나 확인한다

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/python/django/basic-2-3.png" style="border-radius:5%;border:1px solid #e6e1e8"/>
</p>

<br>

😺 베이스가 될 프로젝트를 하나 생성한다 `$ django-admin startproject myproject`

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/python/django/basic-2-4.png" style="border-radius:5%;border:1px solid #e6e1e8"/>
</p>

<br>

😺 서버를 열어본다 

```bash
$ cd ./myproject
$ python manage.py runserver {port주소}
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/python/django/basic-2-5.png" style="border-radius:5%;border:1px solid #e6e1e8"/>
</p>

<br>

> - [참고한 사이트 (유튜부) 🌎](https://www.youtube.com/watch?v=xGdUNyVkAto)
> - [참고한 사이트 (내 옛날 블로그) 🌎](https://easycoding-7.github.io/blog/django/basic/1/#/)