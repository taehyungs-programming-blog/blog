---
layout: default
title: 8. CRUD 중 UPDATE
parent: (Django)
grand_parent: Python
nav_order: 5
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

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
            <li><a href="/update/{id}">update</a></li>
```

```py
@csrf_exempt 
def update(request, id): 
    global topics 
    if request.method == 'GET': 
        for topic in topics: 
            if topic['id'] == int(id): 
                selectedTopic = { 
                    "title":topic['title'], 
                    "body":topic['body'] 
                } 
        article = f''' 
            <form action="/update/{id}/" method="POST"> 
                <p><input type="text" name="title" placeholder="제목" value={selectedTopic["title"]}></p> 
                <p><textarea name="body" placeholder="내용">{selectedTopic["body"]}</textarea></p> 
                <p><input type="submit"></p> 
            </form> 
        ''' 
        return HttpResponse(HTMLTemplate(article, id)) 
    elif request.method == 'POST': 
        title = request.POST['title'] 
        body = request.POST['body'] 
        for topic in topics: 
            if topic['id'] == int(id): 
                topic['title'] = title 
                topic['body'] = body 
        return redirect(f'/read/{id}')
```

<br>

> - [참고한 사이트 (유튜부) 🌎](https://www.youtube.com/watch?v=bycMlzNMuiM&list=PLuHgQVnccGMDLp4GH-rgQhVKqqZawlNwG&index=13)