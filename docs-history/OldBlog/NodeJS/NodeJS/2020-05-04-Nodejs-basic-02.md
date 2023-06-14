---
title: "NodeJS Basic 2강 - Node.js 웹서버 만들기"
permalink: NodeJS/basic-08/                # link 직접 지정
#toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-05-04 00:00:00 -0000
last_modified_at: 2020-11-14 00:00:00 -0000
sidebar:
  title: "Node.js 목차"
  nav: Node.js
tag:
  - Node.js
category:
  - 강의 6
excerpt: ""
classes: wide
header:
  teaser: /file/image/nodejs-page-teaser.gif
---

* [강좌](https://opentutorials.org/course/3332/21032)
* [Github](https://github.com/web-n/web1_html_internet) : 소스기본 리소스

다운로드한 소스코드를 프로젝트의 디렉터리에 옮긴다.

![](/file/image/Nodejs-2_image1.png)

**main.js파일을 만들고 아래 코드를 넣는다.**<br>
코드를 실행한다.<br>

localhost:3000으로 접속해본다.

![](/file/image/Nodejs-2_image2.png)

![](/file/image/Nodejs-2_image3.png)

```js
// main.js
var http = require('http');
var fs = require('fs');
var app = http.createServer(function(request,response){
    var url = request.url;
    if(request.url == '/'){
      url = '/index.html';
    }
    if(request.url == '/favicon.ico'){
      return response.writeHead(404);
    }
    response.writeHead(200);
    console.log(__dirname + url);
    response.end(fs.readFileSync(__dirname + url));
 
});
app.listen(3000);
```

우선은 위 소스를 이해하려하지말고 이런게 있다 Node가 이렇게 동작한다 정도로만 알자.