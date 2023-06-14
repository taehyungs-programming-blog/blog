---
title: "Git : 명령어 정리"
permalink: git/command-summary/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-04-17 00:00:00 -0000
last_modified_at: 2020-04-17 00:00:00 -0000
sidebar:
  title: "etc"
  nav: etc
---

## fetch --nohooks 명령

git history를 복사하지 않음

---

## git branch -r 명령

* 일단 branch는 브랜치를 보는 명령이고
* -r : 원격 저장소 branch의 리스트를본다.
* -a : 로컬, 원격 모든 저장소의 branch를 본다.

> * [참고사이트](https://cjh5414.github.io/get-git-remote-branch/)

---

## 특정 tag clone 하기

```s
$ git clone -b <tag> <url>
# example
$ git clone -b v1.0 git://github.com/source
```

---

## 현재 git 상태 확인 밑 모든파일 스태이지 올리기

```s
$ git status
```

![](/file/image/git_status.png)

```s
$ git add *
```

![](/file/image/git_status2.png)