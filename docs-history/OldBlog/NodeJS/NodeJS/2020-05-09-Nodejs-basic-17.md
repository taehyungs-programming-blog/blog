---
title: "NodeJS Basic 17강 - NodeJS 콘솔에서의 입력값"
permalink: NodeJS/basic-17/                # link 직접 지정
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

* [강좌](https://opentutorials.org/course/3332/21062)
* [Github](https://github.com/8bitscoding/NodeJS-Lector-17)

```js
var args = process.argv;
console.log(args);
```

실행해 보기

```s
node main.js hello
```

![](/file/image/NodeJS-07-1.png)

```
C:\Users\taehy\WebstormProjects\untitled3>node main.js hello
[
  'C:\\Program Files\\nodejs\\node.exe',
  'C:\\Users\\taehy\\WebstormProjects\\untitled3\\main.js',
  'hello'
]
```

```js
var args = process.argv;
console.log(args[2]);
```

```
hello
```