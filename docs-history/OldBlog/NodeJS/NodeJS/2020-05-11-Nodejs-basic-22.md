---
title: "NodeJS Basic 22강 : JS 배열과 반복문"
permalink: NodeJS/basic-22/                # link 직접 지정
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

* [강좌](https://opentutorials.org/course/3332/21121)

```js
var number = [1,400,12,34];
var i = 0;
var total = 0;
while(i < number.length){
  total = total + number[i];
  i = i + 1;
}
console.log(`total : ${total}`);
```