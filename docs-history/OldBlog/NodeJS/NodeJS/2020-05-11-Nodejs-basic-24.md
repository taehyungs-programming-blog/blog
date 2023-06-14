---
title: "NodeJS Basic 24강 : App - 글목록 출력하기"
permalink: NodeJS/basic-24/                # link 직접 지정
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

* [강좌](https://opentutorials.org/course/3332/21123)
* [Github](https://github.com/8bitscoding/NodeJS-Lector-24)

```js
var http = require('http');
var fs = require('fs');
var url = require('url');

var app = http.createServer(function(request,response){
    var _url = request.url;
    var queryData = url.parse(_url, true).query;
    var pathname = url.parse(_url, true).pathname;
    if(pathname === '/'){
        if(queryData.id === undefined){

            fs.readdir('./data', function(error, filelist){
                var title = 'Welcome';
                var description = 'Hello, Node.js';
                var list = '<ul>';
                var i = 0;
                while(i < filelist.length){
                    list = list + `<li><a href="/?id=${filelist[i]}">${filelist[i]}</a></li>`;
                    i = i + 1;
                }
                list = list+'</ul>';
                var template = `
          <!doctype html>
          <html>
          <head>
            <title>WEB1 - ${title}</title>
            <meta charset="utf-8">
          </head>
          <body>
            <h1><a href="/">WEB</a></h1>
            ${list}
            <h2>${title}</h2>
            <p>${description}</p>
          </body>
          </html>
          `;
                response.writeHead(200);
                response.end(template);
            })



        } else {
            fs.readdir('./data', function(error, filelist){
                var title = 'Welcome';
                var description = 'Hello, Node.js';
                var list = '<ul>';
                var i = 0;
                while(i < filelist.length){
                    list = list + `<li><a href="/?id=${filelist[i]}">${filelist[i]}</a></li>`;
                    i = i + 1;
                }
                list = list+'</ul>';
                fs.readFile(`data/${queryData.id}`, 'utf8', function(err, description){
                    var title = queryData.id;
                    var template = `
            <!doctype html>
            <html>
            <head>
              <title>WEB1 - ${title}</title>
              <meta charset="utf-8">
            </head>
            <body>
              <h1><a href="/">WEB</a></h1>
              ${list}
              <h2>${title}</h2>
              <p>${description}</p>
            </body>
            </html>
            `;
                    response.writeHead(200);
                    response.end(template);
                });
            });
        }
    } else {
        response.writeHead(404);
        response.end('Not found');
    }



});
app.listen(3000);
```