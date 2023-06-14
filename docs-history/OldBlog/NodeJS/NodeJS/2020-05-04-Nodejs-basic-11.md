---
title: "NodeJS Basic 11강 - Node.js 파일 읽기"
permalink: NodeJS/basic-11/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-05-04 00:00:00 -0000
last_modified_at: 2020-05-04 00:00:00 -0000
header:
  teaser: /file/image/nodejs-page-teaser.gif
sidebar:
  title: "NodeJS"
  nav: NodeJS
---

* [강의](https://opentutorials.org/course/3332/21048)

![](/file/image/Nodejs-basic-11.png)

```js
var fs = require('fs');
fs.readFile('sample.txt', 'utf8', function(err, data){
    console.log(data);
});
```