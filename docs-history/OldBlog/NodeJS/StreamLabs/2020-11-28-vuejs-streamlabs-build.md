---
title: "StreamLabs 빌드하기"
permalink: vuejs/streamlabs/                # link 직접 지정
#toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-11-28 00:00:00 -0000
last_modified_at: 2020-11-28 00:00:00 -0000
sidebar:
  title: "Vue.JS 목차"
  nav: Vue.JS
tag:
  - Vue.JS
  - Electron
category:
  - StreamLabs
excerpt: ""
classes: wide
header:
  teaser: /file/image/nodejs-page-teaser.gif
---

## 사전사항

* 참고는 https://github.com/stream-labs/streamlabs-obs
* NodeJS(current LTS recommend) : https://nodejs.org/en/
* Yarn(NodeJS에 맞는 버전을 설치해야함) : 

![](/file/image/streamlabs-build-1.png)

홈페이지에 가보면 맞는 버전의 설명이 있음.

```s
$ node -v
```

![](/file/image/streamlabs-build-2.png)

---

## 설치

```s
# install node module
$ yarn install

# compile
$ yarn compile
```

![](/file/image/streamlabs-build-3.png)

---

## 시작

```s
$ yarn start
```

만약 VS Code에서 실행하면 디버거가 자동으로 연결된다고 한다.

![](/file/image/streamlabs-build-4.png)

나머지 자세한 사항은 참고사이트 참고