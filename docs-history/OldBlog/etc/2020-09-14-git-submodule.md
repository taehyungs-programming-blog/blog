---
title: "Git : Submodule"
permalink: git/submodule/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-09-14 00:00:00 -0000
last_modified_at: 2020-09-14 00:00:00 -0000
sidebar:
  title: "etc"
  nav: etc
excerpt: ""
tag:
  - git
category:
  - submodule
header:
  teaser: /file/image/etc-page-teaser.gif
---

* [참고사이트](https://blog.powerumc.kr/449)

* 기존코드 추가

```s
$ git clone <clone-url>
```

* Submodule 추가

```s
$ cd <clone path>
$ git submodule add <submodule-url> <submodule-path>
```

```s
$ git add *
$ git commit -m <commit comment>
$ git push origin master
```

* Submodule 업데이트

```s
$ git submodule update --init --recursive
```

* Submodule 제거

```s
# 특정 Submodule 제거
$ git submodule deinit <submodule-path>

# 모든 Submodule 제거
$ git submodule deinit .
```