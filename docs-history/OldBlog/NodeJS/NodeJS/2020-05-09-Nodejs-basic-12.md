---
title: "NodeJS Basic 12강 - App 파일을 이용해 본문 구현"
permalink: NodeJS/basic-12/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-05-04 00:00:00 -0000
last_modified_at: 2020-05-09 00:00:00 -0000
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

* [강좌](https://opentutorials.org/course/3332/21049)
* [Github](https://github.com/8bitscoding/NodeJS-Lector-12)

여기서 만들고자 하는 것은 Query String에 따라서 본분이 변경되는 홈페이지 최종 버전을 만들고자 한다.

```js
var http = require('http');
var fs = require('fs');
var url = require('url');

var app = http.createServer(function(request,response){
    var _url = request.url;
    var queryData = url.parse(_url, true).query;
    var title = queryData.id;
    if(_url == '/'){
        title = 'Welcome';
    }
    if(_url == '/favicon.ico'){
        return response.writeHead(404);
    }
    response.writeHead(200);

    // Query id를 받아서 그 id대로 파일을 읽는다.
    fs.readFile(`data/${queryData.id}`, 'utf8', function(err, description){
      // 읽어온 파일은 description에 저장하고
      // description은 temp에서 사용된다.
        var template = `
      <!doctype html>
      <html>
      <head>
        <title>WEB1 - ${title}</title>
        <meta charset="utf-8">
      </head>
      <body>
        <h1><a href="/">WEB</a></h1>
        <ul>
          <li><a href="/?id=HTML">HTML</a></li>
          <li><a href="/?id=CSS">CSS</a></li>
          <li><a href="/?id=JavaScript">JavaScript</a></li>
        </ul>
        <h2>${title}</h2>
        <p>${description}</p>
      </body>
      </html>
      `;
        response.end(template);
    })


});
app.listen(3000);
```

이렇게 하면 동적으로 동작하는 웹페이지 작성완료!
