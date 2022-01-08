---
layout: default
title: (Django) 3. django 대략적 구조
parent: (Django)
grand_parent: Python
nav_order: 5
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

django는 아래와 같이 구성된다.

1. Client의 홈페이지 접속요청
2. project의 urls.py에서 접속하고자하는 url을 확인
3. project의 urls.py에서 Client가 접속하고자 하는 app으로 요청
4. 해당 app내에서 urls.py를 확인후 app.view를 통해 Client의 명령을 실행

```
Project(urls.py) ---- app1(urls.py) <-> view.py <-> model.py <-> db
                  |
                  --- app2(urls.py) <-> view.py <-> model.py <-> db
                  |
                  ...
```

😺 아직은 이해하기가 좀 어렵겠지만. Project는 여러개의 app들로 구성되고, Client의 요청을 분배하는 기능은 urls.py를 통해한다. 각 app들은 Client의 요청이 올 경우 view.py에서 해당 요청을 처리 후 응답하게 된다.

😺 우선! app을 하나 만들어 보자 `$ django-admin startapp myapp`

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/python/django/basic-3-1.png" style="border-radius:5%;border:1px solid #e6e1e8"/>
</p>

😺 아직 뭔지는 잘 모르겠지만 위에서 언급한 `views.py` 및 기타 파일들이 생성됨을 알수 있다.(단, `urls.py`는 개발자가 만들어 줘야함)

<br>

> - [참고한 사이트 (유튜부) 🌎](https://www.youtube.com/watch?v=QX8CQMycDa0&list=PLuHgQVnccGMDLp4GH-rgQhVKqqZawlNwG&index=4)