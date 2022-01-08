---
layout: default
title: 7. CRUD 중 DELETE
parent: (Django)
grand_parent: Python
nav_order: 5
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

🐱 Delete 링크를 만들고

```py
def HTMLTemplate(articleTag, id=None):
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
        <ul>
            <a href="/create/">create</a>
            <!--
            링크를 쓰면 안되는게 delete를 클릭시 바로 동작해야하고 GET방식이 아니라 POST방식으로 동작하기 바란다
            <li><a href="/delete/">delete</a></li>
            -->
            <li>
                <form action="/delete/" method="post">
                    <input type="hidden" name="id" value={id}>
                    <input type="submit" value="delete">
                </form>
            </li>
        </ul>
    </body>
    </html>
    '''
```

```py
# myapp.urls.py

urlpatterns = [
    path('', views.index),
    path('create/', views.create),
    path('read/<id>/', views.read),
    path('delete/', views.delete),
]
```

```py
# myapp.view.py

@csrf_exempt
def delete(request):
    global topics
    if request.method == 'POST':
        id = request.POST['id']
        newTopics = []
        for topic in topics:
            if topic['id'] != int(id):
                newTopics.append(topic)
        topics = newTopics
        return redirect('/')
```

🐱 조금 더 수정해보자

```py
def HTMLTemplate(articleTag, id=None):
    global topics
    contextUI = ''
    if id != None:
        contextUI = f'''
            <li>
                <form action="/delete/" method="post">
                    <input type="hidden" name="id" value={id}>
                    <input type="submit" value="delete">
                </form>
            </li>
        '''
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
        <ul>
            <a href="/create/">create</a>
            {contextUI}
        </ul>
    </body>
    </html>
    '''
```

<br>

> - [참고한 사이트 (유튜부) 🌎](https://www.youtube.com/watch?v=bycMlzNMuiM&list=PLuHgQVnccGMDLp4GH-rgQhVKqqZawlNwG&index=12)