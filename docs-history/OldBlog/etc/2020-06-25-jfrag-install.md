---
title: "jFrag artifactory : install"
permalink: jFrag/install/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-06-13 00:00:00 -0000
last_modified_at: 2020-06-13 00:00:00 -0000
sidebar:
  title: "etc"
  nav: etc
excerpt: ""
tag:
  - jFrag
category:
  - install
header:
  teaser: /file/image/etc-page-teaser.gif
---

* [참고사이트](https://www.jfrog.com/confluence/display/JFROG/Installing+Artifactory#InstallingArtifactory-WindowsInstallation)

## JAVA_HOME 설정

![](/file/image/jfrag_intall_01.png)

---

## jFrag 다운로드

* [링크](https://jfrog.com/download-jfrog-platform/)

압출을 풀고 app, var폴더를 artifactory폴더를 만들어 안에 넣는다.

---

## JFROG_HOME 설정

![](/file/image/jfrag_intall_02.png)

---

## jFrag 실행

관리자모드로 cmd실행

```s
$ artifactory.bat
```

`localhost:8082`에 접속

* ID : admin
* PW : password