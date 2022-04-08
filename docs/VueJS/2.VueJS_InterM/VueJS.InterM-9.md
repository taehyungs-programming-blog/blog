---
layout: default
title: "9. 프로젝트 구조화 및 모듈화"
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

* [Get Code 🌎](https://github.com/EasyCoding-7/vue-3.0-intermidiate/tree/structor)

```js
import * as getters from 'store/getters.js'
import * as mutations from 'store/mutations.js'
import * as actions from 'store/actions.js'

export const store = createStore({
	state: {},
    getters: getters,
    mutations: mutations,
    actions: actions
});
```

{% endraw %}