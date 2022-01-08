---
layout: default
title: (Django) 5. CRUD 중 READ
parent: (Django)
grand_parent: Python
nav_order: 5
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## CRUD란?

😸 CRUD는 별거없다. Web에서 많이 사용되는 Create / Read / Update / Delete의 약어이다

## CRUD 중 READ

😸 READ는 간단하다 Client의 요청에 의해서 보여지는 화면 어떤식으로 보여지는지를 의미한다

😸 아래와 같이 View를 구성해보자. 여기서는 READ에 대한 내용이라기 보다 READ를 어떻게 잘 구현할지에 대한 내용

```py
# myapp\views.py

# 기본페이지 접속시 아래 html이 보여진다.
def index(request):
    return HttpResponse('''
    <html>
    <body>
        <h1>Django</h1>
        <ol>
            <li>routing</li>
            <li>view</li>
            <li>model</li>
        </ol>
        <h2>Welcome</h2>
        Hello, Django
    </body>
    </html>
    ''')
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/python/django/basic-5-1.png" style="border-radius:5%;border:1px solid #e6e1e8"/>
</p>

😸 조금 코드를 개선해 보자. 👉 ol 테그의 내부를 개선하고자 한다

```py
# myapp\views.py

topics = [
    {'id':1, 'title':'routing', 'body':'Routing is ...'},
    {'id':2, 'title':'view', 'body':'view is ...'},
    {'id':3, 'title':'model', 'body':'model is ...'},
]

# Create your views here.
def index(request):
    global topics
    ol = ''
    for topic in topics:
        ol += f'<li>{topic["title"]}</li>'

    return HttpResponse(f'''
    <html>
    <body>
        <h1>Django</h1>
        <ol>
            {ol}
        </ol>
        <h2>Welcome</h2>
        Hello, Django
    </body>
    </html>
    ''')
```

😸 조금 더 개선해서 링크까지 걸어보자면

```py
# myapp\views.py

# Create your views here.
def index(request):
    global topics
    ol = ''
    for topic in topics:
        ol += f'<li><a href="/read/{topic["id"]}">{topic["title"]}</a></li>'

    return HttpResponse(f'''
    <html>
```

😸 `HttpResponse` 를 조금 더 함수화 해보자

```py
# myapp\views.py

topics = [
    {'id':1, 'title':'routing', 'body':'Routing is ...'},
    {'id':2, 'title':'view', 'body':'view is ...'},
    {'id':3, 'title':'model', 'body':'model is ...'},
]

def HTMLTemplate(articleTag):
    global topics
    ol = ''
    for topic in topics:
        ol += f'<li><a href="/read/{topic["id"]}">{topic["title"]}</a></li>'
    return f'''
    <html>
    <body>
        <h1>Django</h1>
        <ol>
            {ol}
        </ol>
        {articleTag}
    </body>
    </html>
    '''

# Create your views here.
def index(request):
    article = '''
        <h2>Welcome</h2>
        Hello, Django
    '''
    return HttpResponse(HTMLTemplate(article))


def read(request, id): 
    global topics 
    article = '' 
    for topic in topics: 
        if topic['id'] == int(id): 
            article = f'<h2>{topic["title"]}</h2>{topic["body"]}' 
    return HttpResponse(HTMLTemplate(article))
```

<br>

> - [참고한 사이트 (유튜부) 🌎](https://www.youtube.com/watch?v=7ovAmZjDWmk&list=PLuHgQVnccGMDLp4GH-rgQhVKqqZawlNwG&index=7)
> - [참고한 사이트 (유튜부) 🌎](https://www.youtube.com/watch?v=7ovAmZjDWmk&list=PLuHgQVnccGMDLp4GH-rgQhVKqqZawlNwG&index=8)