---
title: "NodeJS Basic 31강 : HTML - Form"
permalink: NodeJS/basic-31/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-05-12 00:00:00 -0000
last_modified_at: 2020-05-12 00:00:00 -0000
header:
  teaser: /file/image/nodejs-page-teaser.gif
sidebar:
  title: "NodeJS"
  nav: NodeJS
tag:
  - NodeJS
category:
  - Lecture
  - 생활코딩
excerpt: ""
---

* [강좌](https://opentutorials.org/course/3332/21134)

```html
<form action="http://localhost:3000/process_create" method="post">
    <p><input type="text" name="title"></p>
    <p>
        <textarea name="description"></textarea>
    </p>
    <p>
        <input type="submit">
    </p>
</form>
```

![](/file/image/NodeJS-basic-31.png)

이렇게해서 보낼 시(submit을 제출할 시)

```s
http://localhost:3000/process_create?title=123&description=123
```

Query string형태로 보내지게 된다.<br>
그런데 http주소는 전혀 안바뀌는데??<br>
일반 사용자가 볼 수 없는 방식으로 수정이 되기에 그렇다.(method="post" 옵션)