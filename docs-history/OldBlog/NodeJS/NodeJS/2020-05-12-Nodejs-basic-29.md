---
title: "NodeJS Basic 29강 : NodeJS - 동기와 비동기 그리고 콜백"
permalink: NodeJS/basic-29/                # link 직접 지정
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

* [강좌](https://opentutorials.org/course/3332/21132)
* [Github](https://github.com/8bitscoding/NodeJS-basic-29)

```txt
# sample.txt
B
```

```js
var fs = require('fs');
 
/*
//readFileSync
console.log('A');
var result = fs.readFileSync('sample.txt', 'utf8');
console.log(result);
console.log('C');
*/
 // A B C 로 출력됨
 
console.log('A');
fs.readFile('sample.txt', 'utf8', function(err, result){
    console.log(result);
});
console.log('C');
// A C B로 출력됨
```

callback

```js
/*
function a(){
  console.log('A');
}
*/
// 위와 동일한 기능을 한다.
var a = function(){
  console.log('A');
}
 
 
function slowfunc(callback){    // 매개변수로 들어온 함수
  callback();                   // 여기서 실행된다.
}
 
slowfunc(a);
```