---
layout: default
title: "6. Vuex 이론, 설치/등록"
parent: "(Vue.JS 중급)"
grand_parent: "Vue.JS"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

{% raw %}

## MVC , Flux 패턴 비교

🚀 기존 **MVC 패턴의 문제점**?<br>
🚀 아래처럼 MVC로 동작하게 되는데 Model과 View가 늘어남에 따라 서로의 관계가 복잡해 진다.<br>

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/vuejs/2.VueJS_InterM/2.VueJS_InterM-6-1.png"/>
</p>

🚀 그에 반해 Flux는 **Action -> Dispatcher -> Models -> View**의 과정을 거친다.<br>
🚀 Flux의 특징은 **UniDirectional(단방향성)**하다는 점이다.<br>
🚀 Model, View간의 복잡화 되는 현상을 막을 수 있다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/vuejs/2.VueJS_InterM/2.VueJS_InterM-6-1.jpg"/>
</p>

---

## Vuex 필요성?

🚀 **Vuex** - 컴포넌트를 효율적으로 관리하는 라이브러리<br>
🚀 기존 Vue를 쓸 겨우 Component가 많아질경우 자녀의 자녀의 자녀의 .... 이런식으로 Event를 보낼때 **EventBus**가 필요해진다. 단, EventBus를 쓸경우 역시 데이터의 추적이 어려워 진다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/vuejs/2.VueJS_InterM/2.VueJS_InterM-6-2.jpg"/>
</p>

🚀 정리하자면 두 가지 이유에서 Vuex를 쓴다. **Flux 패턴사용**, **EventBus를 피하고자**

---

## Vuex의 컨셉

* **State** - 컴포넌트 간에 공유하는 데이터
* **View** - 데이터를 표시하는 화면
* **Action** - 사용자 입력에 따라 데이터 처리

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/vuejs/2.VueJS_InterM/2.VueJS_InterM-6-2.png"/>
</p>

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/vuejs/2.VueJS_InterM/2.VueJS_InterM-6-3.png"/>
</p>

---

## Vuex 설치 및 등록

* [Get Code 🌎](https://github.com/EasyCoding-7/vue-3.0-intermidiate/tree/initVuex)

```bash
# 최신버전 설치
# $ npm install vuex@next --save

# 버전지정 설치
$ npm install vuex@3.6.2 --save
```

```json
{
  /* */
  "dependencies": {
    "core-js": "^3.8.3",
    "vue": "^3.2.13",
    "vuex": "^4.0.2" // 설치가 되었다면 dependencies아래 나타난다
  },
```

```js
// store/store.js

import Vue from 'vue'
import Vuex from 'vuex'

// 글로벌로 Vuex를 쓴다고 알린다
Vue.use(Vuex);

// store를 쓴다고 등록
export const store = new Vuex.Store({

});
```

```js
// main.js
import { createApp, VueElement } from 'vue'
import App from './App.vue'
import {store} from './store/store'

// createApp(App).mount('#app')

new VueElement({
    el:'#app',
    store,
    render: h => h(App)
})
```

{% endraw %}