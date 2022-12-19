---
layout: default
title: "3. WebPack의 주요 속성"
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

🔥 `webpack.config.js`파일 내부를 보면

```js
module.exports = {
  mode: // ...,
  entry: // ...,
  output: {
    // ...
  },
  loader: {
      // ...
  },
  plugin: {
      // ...
  }
};
```

🔥 이번시간은 각 세부적 설정에 대해 배워본다.

---

## entry

🔥 말 그대로 시작점(진입점)을 의미 C로 말하자면 `main`이다.

```js
module.exports = {
  // ...
  entry: './src/index.js',
  // ...
```

---

## output

🔥 출력의 형식을 정의한다.

```js
module.exports = {
  // ...
  output: {
    filename: 'main.js',
    path: path.resolve(__dirname, 'dist')
  }
  // ...
```

---

## loader

🔥 js파일이 아닌 파일(.css 등)의 해석(.js로 만들어 준다)을 돕는다.

---

## plugin

🔥 webpack으로 생성된 결과물의 형태를 바꿔준다.

---

## Example

* [참고 사이트 🌎](https://joshua1988.github.io/webpack-guide/tutorials/code-splitting.html)

```js
var path = require('path');

module.exports = {
  mode: 'none',
  /*
    * production - 배포용 버전(개발 라이브러리 미 포함)
    * development - 개발용 버전
    * none - 말 그대로 none
  */
  entry: './index.js',
  output: {
    filename: 'bundle.js',
    path: path.resolve(__dirname, 'dist')
  },
  module: {
    rules: [
      {
        // 모든 .css파일에 대하여
        test: /\.css$/,
        // 'style-loader', 'css-loader'를 돌려달라
        use: ['style-loader', 'css-loader']
        // 참고로 use를 쓰면 순서도 중요함 순서는 오른쪽에서 왼쪽순임
        // css를 먼저 읽고 style을 적용해야한다.
      }
    ]
  },
}
```

```js
// webpack.config.js
var path = require('path');
var MiniCssExtractPlugin = require("mini-css-extract-plugin");

module.exports = {
  mode: 'none',
  entry: './index.js',
  output: {
    filename: 'bundle.js',
    path: path.resolve(__dirname, 'dist')
  },
  module: {
    rules: [
      {
        test: /\.css$/,
        use: [
          { loader: MiniCssExtractPlugin.loader },
          "css-loader"
        ]
      }
    ]
  },
  // plugin을 통해서 css를 외부로 빼보자.
  plugins: [
    new MiniCssExtractPlugin()
  ],
}
```

{% endraw %}