---
title: "Jenkins : gradle plugin"
permalink: jenkins/gradleplugin/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-04-19 00:00:00 -0000
last_modified_at: 2020-04-19 00:00:00 -0000
sidebar:
  title: "etc"
  nav: etc
---

* [참고사이트](https://hreeman.tistory.com/m/135)

## gradle plugin 다운로드 / 사용

Jenkins 관리 -> 플러그인 관리 

![](/file/image/jenkins-gradle-plugin-01.png)

Gradle Plugin 다운(아래의 경우는 이미 설치된 경우이다.)

![](/file/image/jenkins-gradle-plugin-02.png)

Jenkins 관리 -> Global Tool Configuration<br>
gradle 이름과 설치 위치를 지정<br>

![](/file/image/jenkins-gradle-plugin-03.png)

사용은 item의 build에서 gradle 탭에서 아래를 입력해주면 된다.

![](/file/image/jenkins-gradle-plugin-04.png)

Task지정 및 Build File지정은 아래와 같이한다.

![](/file/image/jenkins-gradle-plugin-05.png)
