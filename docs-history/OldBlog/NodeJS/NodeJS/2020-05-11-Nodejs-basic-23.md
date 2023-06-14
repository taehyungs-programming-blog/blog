---
title: "NodeJS Basic 23강 : NodeJS 파일 목록 알아내기"
permalink: NodeJS/basic-23/                # link 직접 지정
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

* [강좌](https://opentutorials.org/course/3332/21122)


```js
var testFolder = './data';
var fs = require('fs');
 
fs.readdir(testFolder, function(error, filelist){
  console.log(filelist);
})
```

![](/file/image/Nodejs-basic-23.png)