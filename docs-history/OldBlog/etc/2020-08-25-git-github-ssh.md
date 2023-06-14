---
title: "git : github ssh 등록"
permalink: git/github-ssh/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-08-25 00:00:00 -0000
last_modified_at: 2020-08-25 00:00:00 -0000
sidebar:
  title: "etc"
  nav: etc
excerpt: ""
tag:
  - git
category:
  - github
  - ssh
header:
  teaser: /file/image/etc-page-teaser.gif
---

* [참고사이트](https://brunch.co.kr/@anonymdevoo/10)

## 공개 ssh 키 만들기

```s
$ ssh-keygen
```

![](/file/image/github-ssh-1.png)

다음과 같은 파일이 생성된다.

![](/file/image/github-ssh-2.png)

---

## github에 ssh키 등록하기

`id_rsa.pub`내의 내용을 SSH keys 내에 등록한다.

![](/file/image/github-ssh-3.png)

끝!