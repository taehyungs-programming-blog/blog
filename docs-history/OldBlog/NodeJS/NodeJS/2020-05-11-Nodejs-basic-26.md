---
title: "NodeJS Basic 26강 : JS 함수의 입력"
permalink: NodeJS/basic-26/                # link 직접 지정
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

* [강좌](https://opentutorials.org/course/3332/21124)

```js
console.log(Math.round(1.6)); //2
console.log(Math.round(1.4)); //1
 
function sum(first, second){ // parameter
  console.log(first+second);
}
 
sum(2,4); // argument
```