---
title: "(Jenkins) Git SSH Agent가 잘 동작하지 않을 시 환경변수로 SSH-Key 넣기"
permalink: jenkins/env-ssh-key/                # link 직접 지정
#toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-11-26 00:00:00 -0000
last_modified_at: 2020-11-26 00:00:00 -0000
tag:
  - Jenkins
category:
  - git
  - openssl
sidebar:
  - title: ""
  - nav:
classes: wide
excerpt: ""
header:
  teaser: /file/image/etc-page-teaser.gif
---

Windows Jenkins 서버의 경우 Git SSH-Agent가 잘 동작하지 않는다. 아마 Agent가 기본적으로 Windows에서 실행이 안되고 실행하려해도 Jenkins로 로그인해야하는데 방법이 없다 ... 

그래서 추천하는건 환경변수로 SSH-Key를 등록하는 방법

![](/file/image/jenkins-env-ssh-key.png)

* `GIT_SSH` : C:\Windows\System32\OpenSSH\ssh.exe
* `GIT_SSH_VARIANT` : ssh
* `PATH` : %PATH%;C:\Program Files\Git\cmd;C:\Windows\SysWOW64;C:\Program Files\Git\usr\bin;
* `GIT_SSH_COMMAND` : ssh -o StrictHostKeyChecking=no -i "D:\00. build-gradle-files\ssh-key\th77\id_rsa"