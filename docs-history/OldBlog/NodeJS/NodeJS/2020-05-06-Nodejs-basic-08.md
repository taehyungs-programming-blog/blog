---
title: "NodeJS Basic 8강 - URL로 입력된 값 사용하기"
permalink: NodeJS/basic-08/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-05-06 00:00:00 -0000
last_modified_at: 2020-05-06 00:00:00 -0000
header:
  teaser: /file/image/nodejs-page-teaser.gif
sidebar:
  title: "NodeJS"
  nav: NodeJS
---

* [강의](https://opentutorials.org/course/3332/21047)
* [Github](https://github.com/8bitscoding/NodeJS-Lector-08)

---

아래와 같이 엄청많은 내용 중 특정 내용만 Query String을 이용하여 변경하게 하고 싶다면??

```js
var http = require('http');
var fs = require('fs');
var url = require('url');

var app = http.createServer(function(request,response){
    var _url = request.url;
    var queryData = url.parse(_url, true).query;
    if(request.url == '/'){
        _url = '/index.html';
    }
    if(request.url == '/favicon.ico'){
        return response.writeHead(404);
    }
    response.writeHead(200);
    var template = `
            <!doctype html>
            <html>
            <head>
            <title>WEB1 - ${queryData.id}</title>
            <meta charset="utf-8">
            </head>
            <body>
            <h1><a href="index.html">WEB</a></h1>
        <ol>
        <li><a href="1.html">${queryData.id}</a></li>
        <li><a href="2.html">CSS</a></li>
        <li><a href="3.html">JavaScript</a></li>
        </ol>
        <h2>HTML</h2>
        <p><a href="https://www.w3.org/TR/html5/" target="_blank" title="html5 speicification">Hypertext Markup Language (HTML)</a> is the standard markup language for <strong>creating <u>web</u> pages</strong> and web applications.Web browsers receive HTML documents from a web server or from local storage and render them into multimedia web pages. HTML describes the structure of a web page semantically and originally included cues for the appearance of the document.
        <img src="coding.jpg" width="100%">
            </p><p style="margin-top:45px;">HTML elements are the building blocks of HTML pages. With HTML constructs, images and other objects, such as interactive forms, may be embedded into the rendered page. It provides a means to create structured documents by denoting structural semantics for text such as headings, paragraphs, lists, links, quotes and other items. HTML elements are delineated by tags, written using angle brackets.
            </p>
            </body>
            </html>`;
    response.end(template);
});
app.listen(3000);
```

```s
localhost:3000?id=HELLO
```

![](/file/image/Nodejs-basic-08.png)

위 그림과 같이 타이틀과 1번이 동적으로 변경이 가능해진다.

코드를 조금 더 수정해 보자.

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
    if(request.url == '/favicon.ico'){
        return response.writeHead(404);
    }
    response.writeHead(200);
    var template = `
            <!doctype html>
            <html>
            <head>
            <title>WEB1 - ${title}</title>
            <meta charset="utf-8">
            </head>
            <body>
            <h1><a href="/">WEB</a></h1>
        <ol>
        <li><a href="/?id=HTML">${title}</a></li>
        <li><a href="/?id=CSS">CSS</a></li>
        <li><a href="/?id=JavaScript">JavaScript</a></li>
        </ol>
        <h2>HTML</h2>
        <p><a href="https://www.w3.org/TR/html5/" target="_blank" title="html5 speicification">Hypertext Markup Language (HTML)</a> is the standard markup language for <strong>creating <u>web</u> pages</strong> and web applications.Web browsers receive HTML documents from a web server or from local storage and render them into multimedia web pages. HTML describes the structure of a web page semantically and originally included cues for the appearance of the document.
        <img src="coding.jpg" width="100%">
            </p><p style="margin-top:45px;">HTML elements are the building blocks of HTML pages. With HTML constructs, images and other objects, such as interactive forms, may be embedded into the rendered page. It provides a means to create structured documents by denoting structural semantics for text such as headings, paragraphs, lists, links, quotes and other items. HTML elements are delineated by tags, written using angle brackets.
            </p>
            </body>
            </html>`;
    response.end(template);
});
app.listen(3000);
```
