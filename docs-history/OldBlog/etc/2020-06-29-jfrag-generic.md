---
title: "jFrog artifactory : generic build upload"
permalink: jFrag/generic/                # link 직접 지정
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
  - jFrog
category:
  - generic
header:
  teaser: /file/image/etc-page-teaser.gif
---

* [참고사이트](https://www.jfrog.com/confluence/display/JFROG/Generic+Builds)

일반적 artifactory는 generic이라 생각하자 (jFrog가 대부분 자바 기반 프로젝트라서 C++개발자는 그냥 generic만쓴다고 생각하자)

![](/file/image/jFrog-generic-01.png)

우선 generic repo 생성

![](/file/image/jFrog-generic-02.png)

파일업로드는 curl로 간단하게 처리가능

```s
# curl -u<id>:<passcode> -T "업로드파일명" http://localhost:8081/artifactory/<업로등할 경로>
$ curl -uadmin:AP21Uyjksn77kMy99ngHHFgeFmz -T "C:\gradle\Testbuild\Testbuild.sln" http://localhost:8081/artifactory/generic-local/1.2/Testbuild.sln
```

passcode는 아래버튼을 누르면 얻을 수 있다.

![](/file/image/jFrog-generic-03.png)