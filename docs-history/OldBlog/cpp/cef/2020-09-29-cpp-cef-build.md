---
title: "(cef) build"
permalink: cpp/cef/build/                # link 직접 지정
#toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-09-29 00:00:00 -0000
last_modified_at: 2020-09-29 00:00:00 -0000
sidebar:
  title: "목차"
  nav: cpp
tag:
  - cpp
  - cef
category:
  - build
classes: wide
excerpt: ""
header:
  teaser: /file/image/cpp-page-teaser.gif
---

* [참고사이트](https://bitbucket.org/chromiumembedded/cef-project/src/master/)

---

```s
# cloen code
$ git clone https://bitbucket.org/chromiumembedded/cef-project.git
$ mkdir build
$ cd build
$ cmake -G "Visual Studio 15" -A x64 ..     # for VS2017
$ cmake -G "Visual Studio 16" -A x64 ..     # for VS2019
# VS2019를 추천... VS2017보다 안정적인듯
```

![](/file/image/cpp-cef-1.png)

![](/file/image/cpp-cef-2.png)

빌드해 본다

![](/file/image/cpp-cef-3.png)