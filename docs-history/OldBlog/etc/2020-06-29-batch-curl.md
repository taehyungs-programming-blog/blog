---
title: "Windows Batch : bat curl 쓰기"
permalink: batch/curl/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-06-29 00:00:00 -0000
last_modified_at: 2020-06-29 00:00:00 -0000
sidebar:
  title: "etc"
  nav: etc
excerpt: ""
tag:
  - Windows Batch
category:
  - bat curl
header:
  teaser: /file/image/etc-page-teaser.gif
---

```s
curl -i ^
-H "header1:content1" ^
-H "header2:content2" ^
-d ^"{ ^
    \"json1\": [ \"json1-content\" ], ^
    \"json2\": \"json2=content\" }^" ^
https://url/url
```