---
layout: default
title: "4. WebPack Dev Server"
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

* [참고 사이트 🌎](https://joshua1988.github.io/webpack-guide/tutorials/webpack-dev-server.html)

👾 코드 수정 후 매번 `$ npm run build`를 실행해야할까?<br>
👾 그래서 준비했다. **WebPack Dev Server**!!<br>
👾 별도의 빌드없이 실행이 가능하며, 파일로 떨구지도 않는다(메모리에 보관)

```json
// package.json
{
  // ...
  "scripts": {
    "dev": "webpack serve"
  },
}
```

```bash
$ npm run dev
```

{% endraw %}