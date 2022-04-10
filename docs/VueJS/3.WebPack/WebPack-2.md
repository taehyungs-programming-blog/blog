---
layout: default
title: "2. WebPack이란"
parent: "WebPack"
grand_parent: "Vue.JS"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

{% raw %}

## WebPack 이란?

* [참고 사이트 🌎](https://joshua1988.github.io/webpack-guide/getting-started.html#%EA%B0%9C%EB%B0%9C-%ED%99%98%EA%B2%BD-%EA%B5%AC%EC%84%B1)

👑 **WebPack** - Font-End **모듈 번들러(Module Bundler)**, 간단히 HTML, CSS, JavaScript, Images등을 모듈로 보고 각 모듈을 조합 하나의 웹사이트를 만들어 주는 도구를 **모듈 번들러**라 하며 그 중 하나가 **WebPack**이다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/vuejs/3.WebPack/3.WebPack-2-1.png"/>
</p>

👑 우선 Webpack이 없던 시절은 어떻게 했는지 보자

```bash
# example
$ npm init -y   # npm으로 라이브러리를 관리하겠다.
$ npm i webpack webpack-cli --save-dev  # webpack webpack-cli를 개발용 라이브러리로 설치하겠다
$ npm i lodash
```

👑 **index.html** 하나 생성

```html
<html>
  <head>
    <title>Webpack Demo</title>
    <script src="https://unpkg.com/lodash@4.16.6"></script>
  </head>
  <body>
    <script src="src/index.js"></script>
  </body>
</html>
```

```js
// index.js
function component() {
  var element = document.createElement('div');

  /* lodash is required for the next line to work */
  element.innerHTML = _.join(['Hello','webpack'], ' ');

  return element;
}

document.body.appendChild(component());
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/vuejs/3.WebPack/3.WebPack-2-2.png"/>
</p>

* 현재 확인된 문제점
  * `<script src="https://unpkg.com/lodash@4.16.6"></script>` 이걸 매번 입력해야하나? 버전이 바뀐다면?
  * `<script src="src/index.js"></script>` 파일의 직접경로를 넣는다고? 너무 위험한데?

---

## WebPack으로 문제해결!

👑 위 문제를 **WebPack**으로 해결해 보자.

```json
// package.json
{
  "name": "webpackexample",
  "version": "1.0.0",
  "description": "",
  "main": "index.js",
  "scripts": {
    "test": "echo \"Error: no test specified\" && exit 1",
    // build script 추가
    "build": "webpack --mode=none"
  },

  // ...
```

```html
<!-- index.html 아래와 같이 수정 -->

<html>
  <head>
    <title>Webpack Demo</title>
    <!-- <script src="https://unpkg.com/lodash@4.16.6"></script> -->
  </head>
  <body>
    <!-- <script src="src/index.js"></script> -->
    <script src="dist/main.js"></script>
  </body>
</html>
```

```js
// npm의 lodash를 쓴다고 알린다.
import _ from 'lodash';

function component() {
    var element = document.createElement('div');
  
    /* lodash is required for the next line to work */
    element.innerHTML = _.join(['Hello','webpack'], ' ');
  
    return element;
  }
  
  document.body.appendChild(component());
```

```bash
$ npm run build # Webpack build를 명령
```

👑 기존과 동일하게 웹페이지가 보인다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/vuejs/3.WebPack/3.WebPack-2-3.png"/>
</p>

👑 페이지 요청시에 오직 main.js만 요청함을 알 수 있다.

---

👑 만약 빌드의 결과물을 dist/main.js가 아닌 다른 폴더의 다른 파일명으로 저장하고자 한다면?<br>
👑 `webpack.config.js`를 생성하자

```js
// webpack.config.js
// `webpack` command will pick up this config setup by default
var path = require('path');

module.exports = {
  mode: 'none',
  entry: './src/index.js',
  output: {
    filename: 'main.js',
    path: path.resolve(__dirname, 'dist')
  }
};
```

---

## WebPack의 장점 정리

👑 JavaScript의 유효 범위 설정

```html
<script src="main.js"></srcript>
<!--
var a = 10;
-->
<script src="obj.js"></srcript>
<!--
console.log(a); // 10 <- 이게 되네? -> JS는 이게 가능, WebPack이 유효범위를 파일단위로 끊어준다.
-->
```

👑 **브라우저별 HTTP 요청 숫자 제약** - 브라우저별로 HTTP요청의 갯수제약이 있음, WebPack은 요청의 숫자를 줄이기에 아무래도 속도 측면에서 유리하다.<br>
👑 **사용하지 않는 코드 관리** <br>
👑 + **Dynamic Loading & Lazy Loading 미지원** - 동적으로 사용되는 라이브러리 로딩 가능<br>

<br>

자세한 설명은 다음강에서 설명.

{% endraw %}
