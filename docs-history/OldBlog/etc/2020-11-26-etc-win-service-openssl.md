---
title: "(Windows) Windows Service OpenSSL 확인"
permalink: winp/service-openssl/                # link 직접 지정
#toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-11-26 00:00:00 -0000
last_modified_at: 2020-11-26 00:00:00 -0000
tag:
  - Windows
category:
  - window service
  - openssl
sidebar:
  - title: ""
  - nav:
classes: wide
excerpt: ""
header:
  teaser: /file/image/etc-page-teaser.gif
---

* [참고사이트](https://stackoverflow.com/questions/52113738/starting-ssh-agent-on-windows-10-fails-unable-to-start-ssh-agent-service-erro)

```s
# PowserShell
$ Get-Service ssh-agent
```

서비스 -> OpenSSH Authentication Agent 확인

![](/file/image/service-openssl.png)