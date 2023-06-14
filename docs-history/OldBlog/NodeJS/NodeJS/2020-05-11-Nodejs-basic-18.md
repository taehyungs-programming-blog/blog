---
title: "NodeJS Basic 18강 - Not found 오류 구현"
permalink: NodeJS/basic-18/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-05-11 00:00:00 -0000
last_modified_at: 2020-05-11 00:00:00 -0000
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

* [강좌](https://opentutorials.org/course/3332/21063)
* [Github](https://github.com/8bitscoding/NodeJS-Lector-18)

하고싶은것은 조건문의 활용이다.

```js
// main.js

var http = require('http');
var fs = require('fs');
var url = require('url');

var app = http.createServer(function(request,response){
    var _url = request.url;
    var queryData = url.parse(_url, true).query;
    var title = queryData.id;

    /*
    // 여기를 새로 만들어 보자.
    if(_url == '/'){
        title = 'Welcome';
    }
    if(_url == '/favicon.ico'){
        return response.writeHead(404);
    }
    */

    console.log(url.parse(_url, true).pathname);
    // pathname은 port 주소 뒤부터 querystring을 제외한 주소를 말한다.

    var pathName = url.parse(_url, true).pathname;

    if(pathName === '/') {
        // pathName이 root로 들어오면 정상처리
        fs.readFile(`data/${queryData.id}`, 'utf8', function(err, description){
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
            response.writeHead(200);
            response.end(template);
        })
    } else {
        // 그렇지 않으면 에러처리
        response.writeHead(404);
        response.end('Not found');
    }

});
app.listen(3000);
```

