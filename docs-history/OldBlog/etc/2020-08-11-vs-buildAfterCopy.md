---
title: "jFrog artifactory : 빌드 후 파일복사"
permalink: vs/buildAftercopy/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-08-11 00:00:00 -0000
last_modified_at: 2020-08-11 00:00:00 -0000
sidebar:
  title: "etc"
  nav: etc
excerpt: ""
tag:
  - Visual Studio
category:
  - xcopy
header:
  teaser: /file/image/etc-page-teaser.gif
---

```s
xcopy $(ProjectDir)..\ext\libobs\obs-dll*.dll $(ProjectDir)..\bin\x64\Debug\ /y
```

![](/file/image/vs-buildAfterCopy.png)