---
title: "Gradle : 설치"
permalink: gradle/install/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-04-17 00:00:00 -0000
last_modified_at: 2020-04-17 00:00:00 -0000
sidebar:
  title: "etc"
  nav: etc
---

* [강의](https://www.youtube.com/watch?v=Agf0ZEjUvS8&list=PL7mmuO705dG2pdxCYCCJeAgOeuQN1seZz&index=2)

## 실습환경

* OS : Windows 10(18362.778)
* JDK : 14.0.1
    * 다운로드는 홈페이지에서 하면됨(쉽기에 생략)
    * 뭐 주의할 점이라면 java.exe를 PATH 환경변수에 넣어야한다는 정도
    * 버전확인은 `$ java -version`

![](/file/image/gradle-install-01.png)

* gradle : 6.3
    * 다운로드 홈페이지 : https://gradle.org
    * 홈페이지에 들어가서 쭈~~~욱 내려가면 Installing manually가 있다. 거기서 Download클릭

![](/file/image/gradle-install-02.png)

    * 6.3 complete 클릭

![](/file/image/gradle-install-03.png)

    * C:\Gradle 아래 압축을 푼다.

![](/file/image/gradle-install-04.png)

    * PATH에 Gradle폴더를 넣는다.
    * cmd를 이용하여 gradle 동작을 확인한다. `$ gradle -version`

![](/file/image/gradle-install-05.png)