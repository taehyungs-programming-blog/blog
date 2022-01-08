---
layout: default
title: (Django) 6. CRUD 중 CREATE
parent: (Django)
grand_parent: Python
nav_order: 5
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

😸 Create 페이지를 만들어보자

```py
def HTMLTemplate(articleTag):
    global topics
    ol = ''
    for topic in topics:
        ol += f'<li><a href="/read/{topic["id"]}">{topic["title"]}</a></li>'

    return f'''
    <html>
    <body>
        <h1><a href="/">Django</a></h1>
        <ol>
            {ol}
        </ol>
        {articleTag}
        <a href="/create/">create</a>
    </body>
    </html>
    '''
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/python/django/basic-6-1.png" style="border-radius:5%;border:1px solid #e6e1e8"/>
</p>

<br>

😸 Create의 내부는 아래와 같고, 제출을 클릭시 새로운 페이지가 생성되었으면 한다

```py
def create(request):
    article = '''
        <form action="/create/">
            <p><input type="text" name="title" placeholder="제목"></p>
            <p><textarea name="body" placeholder="내용"></textarea></p>
            <p><input type="submit"></p>
        </form>
    '''
    return HttpResponse(HTMLTemplate(article))
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/python/django/basic-6-1.png" style="border-radius:5%;border:1px solid #e6e1e8"/>
</p>

😸 제출시 특이한 점은 `http://127.0.0.1:8000/create/?title=CRUD&body=CRUD` 이런식(query string, GET방식)으로 요청을 보내게 되는데 사용자의 정보를 url에 싣는게 합리적인 방법일까?

🐱 GET 방식 말고 POST방식으로 개선해보자

```py
# csrf 보안기능을 꺼놔야 한다
from django.views.decorators.csrf import csrf_exempt

def create(request):
    # 간단하게 method="POST를 추가함으로 개선가능
    article = '''
        <form action="/create/" method="POST">
            <p><input type="text" name="title" placeholder="제목"></p>
            <p><textarea name="body" placeholder="내용"></textarea></p>
            <p><input type="submit"></p>
        </form>
    '''
    return HttpResponse(HTMLTemplate(article))
```

## GET, POST 구현

```py
nextId = 4

# ...

from django.views.decorators.csrf import csrf_exempt

@csrf_exempt
def create(request):
    global nextId
    if request.method == 'GET':
        article = '''
            <form action="/create/" method="POST">
                <p><input type="text" name="title" placeholder="제목"></p>
                <p><textarea name="body" placeholder="내용"></textarea></p>
                <p><input type="submit"></p>
            </form>
        '''
        return HttpResponse(HTMLTemplate(article))
    elif request.method == 'POST':
        title = request.POST['title']
        body = request.POST['body']
        newTopic = {"id":nextId ,"title":title, "body":body}
        nextId += 1
        topics.append(newTopic)
        return HttpResponse(HTMLTemplate('AAA'))
```

🐱 응답을 의미없는 AAA를 보낼게 아니라 좀 더 의미있는 값을 보내보자

```py
@csrf_exempt
def create(request):
    global nextId
    if request.method == 'GET':
        article = '''
            <form action="/create/" method="POST">
                <p><input type="text" name="title" placeholder="제목"></p>
                <p><textarea name="body" placeholder="내용"></textarea></p>
                <p><input type="submit"></p>
            </form>
        '''
        return HttpResponse(HTMLTemplate(article))
    elif request.method == 'POST':
        title = request.POST['title']
        body = request.POST['body']
        newTopic = {"id":nextId ,"title":title, "body":body}
        topics.append(newTopic)
        url = '/read/'+str(nextId)
        nextId += 1
        return redirect(url)
```

<br>

> - [참고한 사이트 (유튜부) 🌎](https://www.youtube.com/watch?v=MZqIjlJqeR0&list=PLuHgQVnccGMDLp4GH-rgQhVKqqZawlNwG&index=9)
> - [참고한 사이트2 (유튜부) 🌎](https://www.youtube.com/watch?v=JVoKGD9nrY0&list=PLuHgQVnccGMDLp4GH-rgQhVKqqZawlNwG&index=10)
> - [참고한 사이트3 (유튜부) 🌎](https://www.youtube.com/watch?v=rIr0TAVgrS4&list=PLuHgQVnccGMDLp4GH-rgQhVKqqZawlNwG&index=11)