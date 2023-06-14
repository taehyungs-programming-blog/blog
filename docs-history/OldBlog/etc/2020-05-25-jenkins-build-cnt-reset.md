---
title: "Jenkins : item Build Count reset"
permalink: jenkins/reset-build-cnt/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-05-25 00:00:00 -0000
last_modified_at: 2020-05-25 00:00:00 -0000
sidebar:
  title: "etc"
  nav: etc
excerpt: ""
tag:
  - Jenkins
category:
  - build
  - cnt
  - reset
header:
  teaser: /file/image/etc-page-teaser.gif
---

Jenkins 설정 -> Script Console 아래 작성

```s
def jobName = "Enter Job Name"
def job = Jenkins.instance.getItem(jobName)
job.getBuilds().each { it.delete() }
job.nextBuildNumber = 1
job.save()
```

* [참고사이트](https://linuxhelp4u.blogspot.com/2018/03/how-to-clean-or-reset-build-numbers-in.html)